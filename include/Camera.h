#pragma once

/*用矩形的长宽*/
#define IsInRect_WH(x,y,rx,ry,rw,rh) ((x)>=(rx)&&(y)>=(ry)&&(x)<(rx)+(rw)&&(y)<(ry)+(rh))
/*用矩形的左上与右下点，这个快点*/
#define IsInRect_LR(x,y,lx,ly,rx,ry) ((x)>=(lx)&&(y)>=(ly)&&(x)<=(rx)&&(y)<=(ry))
/*获取结束点的坐标*/
#define GetEnd(v,l) ((v)+(l)-1)

/*将WorldPosition变为CameraPosition*/
class Camera
{
public:

	size_t x, y; //摄像机坐标

	inline Camera()noexcept;
	inline Camera(size_t x, size_t y)noexcept;

	/*设置摄像机位置*/
	inline void SetPosition(const size_t& x, const size_t& y)noexcept;

	/*移动摄像机*/
	inline void Move(int x, int y)noexcept;

	/*获取屏幕坐标*/
	inline void ScreenPosition(int& x, int& y)noexcept;
	inline int ScreenX(int x)noexcept;
	inline int ScreenY(int y)noexcept;

	/*获取屏幕坐标，可能会溢出*/
	inline void ScreenPosition(size_t& x, size_t& y)noexcept;

	/*获取世界坐标*/
	inline void WorldPositon(size_t& x, size_t& y)noexcept;
	inline size_t WorldX(size_t x)noexcept;
	inline size_t WorldY(size_t y)noexcept;

};
inline Camera::Camera() noexcept :
	x(0), y(0)
{}

inline Camera::Camera(size_t x, size_t y) noexcept :
	x(x), y(y)
{}

inline void Camera::SetPosition(const size_t& x, const size_t& y) noexcept
{
	this->x = x;
	this->y = y;
}

inline void Camera::Move(int x, int y) noexcept
{
	if (x < 0 && this->x < (size_t)-x)
		this->x = 0;
	else
		this->x += x;
	if (y < 0 && this->y < (size_t)-y)
		this->y = 0;
	else
		this->y += y;
}

inline void Camera::ScreenPosition(int& x, int& y) noexcept
{
	x -= this->x;
	y -= this->y;
}

inline int Camera::ScreenX(int x) noexcept
{
	return x - this->x;
}

inline int Camera::ScreenY(int y) noexcept
{
	return y - this->y;
}

inline void Camera::ScreenPosition(size_t& x, size_t& y) noexcept
{
	x -= this->x;
	y -= this->y;
}

inline void Camera::WorldPositon(size_t& x, size_t& y) noexcept
{
	x += this->x;
	y += this->y;
}

inline size_t Camera::WorldX(size_t x) noexcept
{
	return this->x + x;
}

inline size_t Camera::WorldY(size_t y) noexcept
{
	return this->y + y;
}
