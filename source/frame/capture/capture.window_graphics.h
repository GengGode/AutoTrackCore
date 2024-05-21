#pragma once
#include "capture.include.h"
#include "utils/utils.window_graphics.h"
#include "utils/utils.window_scale.h"
#include <global/global.include.h>
#include <utils/convect.string.h>

namespace tianli::frame::capture
{
    class capture_window_graphics : public capture_source
    {
        winrt::com_ptr<IInspectable> m_device{ nullptr };
        winrt::com_ptr<ID3D11DeviceContext> m_d3dContext{ nullptr };
        winrt::Windows::Graphics::Capture::GraphicsCaptureItem m_item{ nullptr };
        winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool m_framePool{ nullptr };
        winrt::Windows::Graphics::Capture::GraphicsCaptureSession m_session{ nullptr };
        winrt::Windows::Graphics::SizeInt32 m_lastSize;
        winrt::com_ptr<IDXGISwapChain1> m_swapChain{ nullptr };

    public:
        capture_window_graphics(std::shared_ptr<global::logger> logger = nullptr) : capture_source(logger)
        {
            this->type = source_type::window_graphics;
            auto init_global = utils::window_graphics::graphics_global::get_instance();
        }
        ~capture_window_graphics() override { uninitialized(); }

    public:
        bool initialization() override
        {
            if (this->is_initialized)
                return true;

            if (this->is_callback)
                this->source_handle = this->source_handle_callback();
            if (IsWindow(this->source_handle) == false)
                return false;

            m_device = utils::window_graphics::CreateDirect3DDevice(utils::window_graphics::graphics_global::get_instance().dxgi_device.get());
            m_item = utils::window_graphics::CreateCaptureItemForWindow(this->source_handle);

            if (m_item == nullptr)
                return false;

            m_lastSize = m_item.Size();
            m_swapChain = utils::window_graphics::CreateDXGISwapChain(utils::window_graphics::graphics_global::get_instance().d3d_device, static_cast<uint32_t>(m_lastSize.Width),
                                                                      static_cast<uint32_t>(m_lastSize.Height), DXGI_FORMAT_B8G8R8A8_UNORM, 2);

            utils::window_graphics::graphics_global::get_instance().d3d_device->GetImmediateContext(m_d3dContext.put());

            m_framePool = winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool::Create(m_device.as<winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice>(),
                                                                                                static_cast<winrt::Windows::Graphics::DirectX::DirectXPixelFormat>(87), 2, m_lastSize);
            m_session = m_framePool.CreateCaptureSession(m_item);

            utils::window_graphics::set_capture_session_property(m_session);

            m_session.StartCapture();

            this->is_initialized = true;
            return true;
        }

        bool uninitialized() override
        {
            if (this->is_initialized == false)
                return true;

            if (m_session != nullptr)
                m_session.Close();
            if (m_framePool != nullptr)
                m_framePool.Close();
            m_session = nullptr;
            m_framePool = nullptr;
            m_swapChain = nullptr;
            m_item = nullptr;

            this->is_initialized = false;
            return true;
        }

        bool set_capture_handle(HWND handle = 0) override
        {
            if (handle == nullptr)
                return false;
            if (handle == this->source_handle)
                return true;
            if (uninitialized() == false)
                return false;
            this->source_handle = handle;
            if (initialization() == false)
                return false;
            this->is_callback = false;
            return true;
        }

        bool set_source_handle_callback(std::function<HWND()> callback) override
        {
            if (callback == nullptr)
                return false;
            if (uninitialized() == false)
                return false;
            this->source_handle_callback = callback;
            if (initialization() == false)
                return false;
            this->is_callback = true;
            return true;
        }

        bool get_frame(cv::Mat& frame) override;
        
    private:
        RECT source_rect = { 0, 0, 0, 0 };
        RECT source_client_rect = { 0, 0, 0, 0 };
        cv::Size source_client_size;
    };

} // namespace tianli::frame::capture
