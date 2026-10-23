#pragma once
// Substitui D3DX11CreateShaderResourceViewFromMemory, que exigia o DirectX SDK de
// 2010 (descontinuado). Usa WIC + D3D11, que ja vem no Windows SDK.
//
// Todas as 99 imagens do projeto sao PNG (verificado pelos magic bytes em
// Images.hpp), entao o WIC le todas. Se algum dia entrar um DDS, o WIC nao le e
// esta funcao devolve false - adicione o caminho DDS nessa hora.
#include <d3d11.h>
#include <wincodec.h>
#include <vector>
#include <cstdint>

#pragma comment(lib, "windowscodecs.lib")
// CoInitializeEx/CoCreateInstance vivem aqui; sem isso o link falha com
// __imp_CoCreateInstance nao resolvido.
#pragma comment(lib, "ole32.lib")

namespace Texture
{
    inline bool LoadFromMemory( ID3D11Device* device , const void* data , size_t size ,
                                ID3D11ShaderResourceView** out ) {
        if ( !device || !data || size == 0 || !out ) return false;
        *out = nullptr;

        IWICImagingFactory* factory = nullptr;
        if ( FAILED( CoCreateInstance( CLSID_WICImagingFactory , nullptr , CLSCTX_INPROC_SERVER ,
                                       IID_PPV_ARGS( &factory ) ) ) ) {
            return false;
        }

        IWICStream* stream = nullptr;
        IWICBitmapDecoder* decoder = nullptr;
        IWICBitmapFrameDecode* frame = nullptr;
        IWICFormatConverter* converter = nullptr;
        ID3D11Texture2D* texture = nullptr;
        bool ok = false;

        // WICStream::InitializeFromMemory recebe BYTE*, nao const - a funcao nao escreve.
        if ( SUCCEEDED( factory->CreateStream( &stream ) ) &&
             SUCCEEDED( stream->InitializeFromMemory( ( BYTE* ) data , ( DWORD ) size ) ) &&
             SUCCEEDED( factory->CreateDecoderFromStream( stream , nullptr , WICDecodeMetadataCacheOnLoad , &decoder ) ) &&
             SUCCEEDED( decoder->GetFrame( 0 , &frame ) ) ) {

            if ( SUCCEEDED( factory->CreateFormatConverter( &converter ) ) &&
                 // BGRA e o formato que o D3D11 quer para R8G8B8A8 em textura de UI;
                 // sem essa conversao os canais vermelho e azul saem trocados.
                 SUCCEEDED( converter->Initialize( frame , GUID_WICPixelFormat32bppBGRA ,
                                                   WICBitmapDitherTypeNone , nullptr , 0.0 ,
                                                   WICBitmapPaletteTypeCustom ) ) ) {
                UINT w = 0 , h = 0;
                if ( SUCCEEDED( converter->GetSize( &w , &h ) ) && w > 0 && h > 0 ) {
                    std::vector<uint8_t> pixels( ( size_t ) w * h * 4 );
                    if ( SUCCEEDED( converter->CopyPixels( nullptr , w * 4 , ( UINT ) pixels.size( ) , pixels.data( ) ) ) ) {
                        D3D11_TEXTURE2D_DESC desc = {};
                        desc.Width = w;
                        desc.Height = h;
                        desc.MipLevels = 1;
                        desc.ArraySize = 1;
                        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
                        desc.SampleDesc.Count = 1;
                        desc.Usage = D3D11_USAGE_DEFAULT;
                        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

                        D3D11_SUBRESOURCE_DATA sub = {};
                        sub.pSysMem = pixels.data( );
                        sub.SysMemPitch = w * 4;

                        if ( SUCCEEDED( device->CreateTexture2D( &desc , &sub , &texture ) ) ) {
                            D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};
                            srv.Format = desc.Format;
                            srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
                            srv.Texture2D.MipLevels = 1;
                            ok = SUCCEEDED( device->CreateShaderResourceView( texture , &srv , out ) );
                        }
                    }
                }
            }
        }

        if ( texture ) texture->Release( );
        if ( converter ) converter->Release( );
        if ( frame ) frame->Release( );
        if ( decoder ) decoder->Release( );
        if ( stream ) stream->Release( );
        if ( factory ) factory->Release( );
        return ok;
    }
}
