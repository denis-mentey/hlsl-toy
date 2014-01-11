#include "window.h"

namespace toy {

namespace {
	static const wchar_t* WINDOW_CLASS = L"hlsl-toy-class";
} // anonymous namespace

LRESULT WINAPI Window::windows_proc(HWND handle, UINT message, WPARAM wparam, LPARAM lparam) {
	Window* window = reinterpret_cast<Window*>(::GetWindowLong(handle, GWL_USERDATA));

	if (window == nullptr || window->is_closing()) {
		return DefWindowProcW(handle, message, wparam, lparam);
	}

	switch (message) {
		case WM_CLOSE:
			window->handle_close();
			break;

		default:
			return DefWindowProcW(handle, message, wparam, lparam);
	}
	
	return 0;
}

void Window::register_class(HINSTANCE instance) {
	WNDCLASSW window_class;

	window_class.style = 0;
	window_class.lpfnWndProc = Window::windows_proc;
	window_class.cbClsExtra = 0;
	window_class.cbWndExtra = 0;
	window_class.hInstance = instance;
	window_class.hIcon = 0;
	window_class.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
	window_class.hbrBackground = 0;
	window_class.lpszMenuName = 0;
	window_class.lpszClassName = WINDOW_CLASS;

	::RegisterClassW(&window_class);
}

Window::Window()
	: _handle(0)
	, _is_closing(false)
{}

Window::~Window() {
	if (_handle) {
		close();
	}
}

void Window::open(HINSTANCE instance, const wchar_t* title, const unsigned w, const unsigned h) {
	RECT desktop_rect;
	::GetClientRect(::GetDesktopWindow(), &desktop_rect);
	unsigned x = (desktop_rect.right - w) / 2;
	unsigned y = (desktop_rect.right - w) / 2;

	RECT windows_rect;
	windows_rect.left = x;
	windows_rect.right = windows_rect.left + w;
	windows_rect.top = y;
	windows_rect.bottom = windows_rect.top + h;

	_handle = ::CreateWindowW(WINDOW_CLASS, title, 0, x, y, w, h, 0, 0, instance, 0);
	::SetWindowLong(_handle, GWL_USERDATA, reinterpret_cast<LONG>(this));

	::ShowWindow(_handle, SW_SHOW);
	::UpdateWindow(_handle);
}

void Window::close() {
	::SetWindowLong(_handle, GWL_USERDATA, 0);
	::DestroyWindow(_handle);
	_handle = 0;
}

void Window::update() {
	MSG message;
	while (::PeekMessage(&message, _handle, 0, 0, PM_REMOVE)) {
		::TranslateMessage(&message);
		::DispatchMessage(&message);
	}
}

bool Window::is_closing() const {
	return _is_closing;
}

HWND Window::handle() const {
	return _handle;
}

void Window::handle_close() {
	_is_closing = true;
}

} // namespace toy