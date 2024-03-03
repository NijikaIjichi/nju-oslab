#ifndef __ASSERT_H__
#define __ASSERT_H__

int abort(const char *, int) __attribute__((noreturn));

/* assert: 断言条件为真，若为假则蓝屏退出 */
#define assert(cond) \
	((cond) ? (0) : (abort(__FILE__, __LINE__)))

#endif
