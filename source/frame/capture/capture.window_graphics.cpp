#include "capture.window_graphics.h"

namespace tianli::frame::capture
{
    bool capture_window_graphics::get_frame(cv::Mat& frame)
    {
        static ID3D11Texture2D* bufferTexture;

        if (this->is_callback)
            set_capture_handle(this->source_handle_callback());

        if (m_framePool == nullptr)
        {
            uninitialized();
            if (initialization() == false)
                return false;
        }

        winrt::Windows::Graphics::Capture::Direct3D11CaptureFrame new_frame{ nullptr };

        new_frame = m_framePool.TryGetNextFrame();
        if (new_frame == nullptr)
            return false;

        auto frame_size = new_frame.ContentSize();
        auto& desc = utils::window_graphics::graphics_global::get_instance().desc_type;
        if (desc.Width != static_cast<UINT>(m_lastSize.Width) || desc.Height != static_cast<UINT>(m_lastSize.Height))
        {
            desc.Width = m_lastSize.Width;
            desc.Height = m_lastSize.Height;
        }

        if (frame_size.Width != m_lastSize.Width || frame_size.Height != m_lastSize.Height)
        {
            m_framePool.Recreate(m_device.as<winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice>(), winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized, 2,
                                 frame_size);
            m_lastSize = frame_size;

            m_swapChain->ResizeBuffers(2, static_cast<uint32_t>(m_lastSize.Width), static_cast<uint32_t>(m_lastSize.Height),
                                       static_cast<DXGI_FORMAT>(winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized), 0);
        }
        auto frameSurface = utils::window_graphics::GetDXGIInterfaceFromObject<ID3D11Texture2D>(new_frame.Surface());

        // auto d3dDevice = GetDXGIInterfaceFromObject<ID3D11Device>(m_device);
        utils::window_graphics::graphics_global::get_instance().d3d_device->CreateTexture2D(&desc, nullptr, &bufferTexture);
        D3D11_BOX client_box;
        bool client_box_available = utils::window_graphics::get_client_box(this->source_handle, desc.Width, desc.Height, &client_box);

        if (client_box_available)
        {
            m_d3dContext->CopySubresourceRegion(bufferTexture, 0, 0, 0, 0, frameSurface.get(), 0, &client_box);
        }
        else
        {
            m_d3dContext->CopyResource(bufferTexture, frameSurface.get());
        }

        if (bufferTexture == nullptr)
        {
            return false;
        }

        D3D11_MAPPED_SUBRESOURCE mappedTex;
        m_d3dContext->Map(bufferTexture, 0, D3D11_MAP_READ, 0, &mappedTex);

        auto data = mappedTex.pData;
        auto pitch = mappedTex.RowPitch;
        if (data == nullptr)
            return false;

        frame = cv::Mat(frame_size.Height, frame_size.Width, CV_8UC4, data, pitch);
        if (client_box_available)
        {
            if (client_box.right - client_box.left > frame_size.Width || client_box.bottom - client_box.top > frame_size.Height)
                return false;
            this->source_frame = frame(cv::Rect(0, 0, client_box.right - client_box.left, client_box.bottom - client_box.top));
        }
        // 释放资源
        bufferTexture->Release();

        if (this->source_frame.empty())
            return false;
        if (this->source_frame.cols < 480 || this->source_frame.rows < 360)
            return false;
        frame = this->source_frame;
        return true;
}
}