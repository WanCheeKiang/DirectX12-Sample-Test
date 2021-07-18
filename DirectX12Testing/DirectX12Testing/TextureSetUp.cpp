#include "TextureSetUp.h"
#include <DDSTextureLoader.h>
#include <ResourceUploadBatch.h>
using namespace DirectX;

void TextureSetUp::Create2dTexture(UINT textureWidth, UINT textureHeight, ComPtr<ID3D12Resource>& texture, ComPtr<ID3D12Device2> device)
{
    D3D12_RESOURCE_DESC textureDesc = {};
    textureDesc.MipLevels = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.Width = textureWidth;
    textureDesc.Height = textureHeight;
    textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
    textureDesc.DepthOrArraySize = 1;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

    //it seems like it's affecting in alot of section, have to think about getting a eairer way to get the var for it.  TODO
    CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT); 
    ThrowIfFailed(device->CreateCommittedResource(
        &HeapProperties::Default,
        D3D12_HEAP_FLAG_NONE,
        &textureDesc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&texture)));
}

ComPtr<ID3D12Resource> TextureSetUp::GetTexture()
{
    return ComPtr<ID3D12Resource>(); //temp
}

void TextureSetUp::CreateGPUUploadBuffer(ComPtr<ID3D12Resource>& texture, ComPtr<ID3D12Device2> device)
{
    //ComPtr<ID3D12Resource> texture = GetTexture(); ////temp 
    
    const UINT64 uploadBufferSize = GetRequiredIntermediateSize(texture.Get(), 0, 1);
    
    // Create the GPU upload buffer.
    CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
    ThrowIfFailed(device->CreateCommittedResource(
        &heapProp,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&textureUploadHeap)));


}

void TextureSetUp::CreateRTVDescriptorHeap(ComPtr<ID3D12Device2> device, ComPtr<ID3D12Resource> rtv, float frameCount, UINT m_rtvDescriptorSize)
{
	// Create rtv descriptor heaps.
    // Describe and create a render target view (RTV) descriptor heap.
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = frameCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    ThrowIfFailed(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtv)));


    m_rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
}

void TextureSetUp::CreateSRV(ComPtr<ID3D12Device2> device, D3D12_RESOURCE_DESC* textureDesc)
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = textureDesc->Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    device->CreateShaderResourceView(m_texture.Get(), &srvDesc, m_srvDescHeap->GetCPUDescriptorHandleForHeapStart());
}

void TextureSetUp::CretaeSRVDescriptorHeap(ComPtr<ID3D12Device2> device, float frameCount, UINT m_rtvDescriptorSize)
{
    // Describe and create a shader resource view(SRV) heap for the texture.
    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
    srvHeapDesc.NumDescriptors = 1;
    srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    ThrowIfFailed(device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_srvDescHeap)));

}

void TextureSetUp::CreateCBV(ComPtr<ID3D12Device2> device)
{
    CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(1024 * 64); // size of the resource heap. Must be a multiple of 64KB for single-textures and constant buffers
    ThrowIfFailed(device->CreateCommittedResource(
        &HeapProperties::Upload,// this heap will be used to upload the constant buffer data
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_constantBuffer)));

    // Describe and create a constant buffer view.
    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
    cbvDesc.BufferLocation = m_constantBuffer->GetGPUVirtualAddress();
    cbvDesc.SizeInBytes = (sizeof(ConstantBuffer) + 255) & ~255;    // CB size is required to be 256-byte aligned.
    device->CreateConstantBufferView(&cbvDesc, m_cbvHeap->GetCPUDescriptorHandleForHeapStart());

    // Map and initialize the constant buffer. We don't unmap this until the
    // app closes. Keeping things mapped for the lifetime of the resource is okay.
    ZeroMemory(&m_constantBufferData, sizeof(m_constantBufferData));
    CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
    ThrowIfFailed(m_constantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&m_pCbvDataBegin)));
    memcpy(m_pCbvDataBegin, &m_constantBufferData, sizeof(m_constantBufferData));
}

void TextureSetUp::UpdateCBV()
{
    const float translationSpeed = 0.005f;
    const float offsetBounds = 1.25f;

    m_constantBufferData.offset.x += translationSpeed;
    if (m_constantBufferData.offset.x > offsetBounds)
    {
        m_constantBufferData.offset.x = -offsetBounds;
    }
    memcpy(m_pCbvDataBegin, &m_constantBufferData, sizeof(m_constantBufferData));
}

void TextureSetUp::LoadTextureData(ComPtr<ID3D12Device2> device, ComPtr<ID3D12GraphicsCommandList>& commandList, const wchar_t* szFile, const UINT& cbvIndex, ComPtr<ID3D12Resource>& buffer, ComPtr<ID3D12Resource>& upload,UINT32 cbvDescriptorSize)
{
    HRESULT hr = E_NOTIMPL;
    ComPtr<ID3D12Device> d;
    device.As(&d);
    ResourceUploadBatch temp(d.Get());

    bool generateMipsIfMissing = false;
    bool isCubeMap;
    DDS_ALPHA_MODE ddsAlphaMode;
    CreateDDSTextureFromFile(d.Get(), temp, szFile, textureUploadHeap.GetAddressOf(), generateMipsIfMissing, 0, &ddsAlphaMode,&isCubeMap);

    //TexMetadata metadata;
    //ScratchImage image;
    
    // This comes from the DirectXTK library


    //std::vector<D3D12_SUBRESOURCE_DATA> textureData = {};
    //for (size_t arrayIndex = 0; arrayIndex < metadata.arraySize; ++arrayIndex)
    //{
    //    for (size_t mipIndex = 0; mipIndex < metadata.mipLevels; mipIndex++)
    //    {
    //        const Image* currentImage = image.GetImage(mipIndex, arrayIndex, 0);
    //        D3D12_SUBRESOURCE_DATA srd = { 0 };
    //        srd.pData = currentImage->pixels;
    //        srd.RowPitch = currentImage->rowPitch;
    //        srd.SlicePitch = currentImage->slicePitch;
    //        textureData.push_back(srd);
    //    }
    //}

    //Describe and create a Texture2D.
    //CD3DX12_RESOURCE_DESC textureDesc = CD3DX12_RESOURCE_DESC(
    //    (D3D12_RESOURCE_DIMENSION)metadata.dimension,
    //    0,
    //    static_cast<UINT>(metadata.width),
    //    static_cast<UINT>(metadata.height),
    //    static_cast<UINT16>(metadata.arraySize),
    //    static_cast<UINT16>(metadata.mipLevels),
    //    metadata.format,
    //    1,
    //    0,
    //    D3D12_TEXTURE_LAYOUT_UNKNOWN,
    //    D3D12_RESOURCE_FLAG_NONE);

    //CD3DX12_HEAP_PROPERTIES prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    //ThrowIfFailed(device->CreateCommittedResource(
    //    &prop,
    //    D3D12_HEAP_FLAG_NONE,
    //    &textureDesc,
    //    D3D12_RESOURCE_STATE_COPY_DEST,
    //    nullptr,
    //    IID_PPV_ARGS(&buffer)));

    //UINT64 uploadBufferSize = GetRequiredIntermediateSize(buffer.Get(), 0, static_cast<UINT16>(metadata.arraySize) * static_cast<UINT16>(metadata.mipLevels));

    //// Create the GPU upload buffer.
    //prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    //CD3DX12_RESOURCE_DESC resource_desc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
    //ThrowIfFailed(device->CreateCommittedResource(
    //    &prop,
    //    D3D12_HEAP_FLAG_NONE,
    //    &resource_desc,
    //    D3D12_RESOURCE_STATE_GENERIC_READ,
    //    nullptr,
    //    IID_PPV_ARGS(&upload)));

    //UpdateSubresources(
    //    commandList.Get(),
    //    buffer.Get(),
    //    upload.Get(),
    //    0,
    //    0,
    //    static_cast<UINT16>(metadata.arraySize) * static_cast<UINT16>(metadata.mipLevels),
    //    textureData.data());

    //CD3DX12_RESOURCE_BARRIER resource_barrier =
    //    CD3DX12_RESOURCE_BARRIER::Transition(
    //        buffer.Get(),
    //        D3D12_RESOURCE_STATE_COPY_DEST,
    //        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    //commandList->ResourceBarrier(1, &resource_barrier);

    //// Describe and create a SRV for the texture.
    //D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    //srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    //srvDesc.Format = textureDesc.Format;
    //srvDesc.ViewDimension = (!metadata.IsCubemap()) ? D3D12_SRV_DIMENSION_TEXTURE2D : D3D12_SRV_DIMENSION_TEXTURECUBE;
    //srvDesc.TextureCube.MipLevels = static_cast<UINT>(metadata.mipLevels);

    //CD3DX12_CPU_DESCRIPTOR_HANDLE cbvHandle(m_cbvHeap->GetCPUDescriptorHandleForHeapStart(), cbvIndex, cbvDescriptorSize);
    //device->CreateShaderResourceView(buffer.Get(), &srvDesc, cbvHandle);

    //commandList->DiscardResource(upload.Get(), nullptr);

}


