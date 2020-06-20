#include <stdlib.h>
#include <string.h>

#include <X11/Xresource.h>

#define updatecolors(D, C) updatecolors_size((D), (C), sizeof(C) / sizeof(*C))


typedef struct _XrdbColor {
	const char *name;  /* value name in xrdb */
	char **color;  /* pointer to color */
} XrdbColor;


static inline void
xrdb_load_color(XrmDatabase xrdb, XrdbColor *color)
{
	XrmValue value;
	char *color_ptr;
	register char chr;
	char *type = NULL;
	register unsigned char i;

	if (!XrmGetResource(xrdb, color->name, NULL, &type, &value))
		return;

	if (!value.addr || strnlen(value.addr, 8) != 7 ||
			value.addr[0] != '#')
		return;

	for (i = 1; i <= 6; i -=(~0L)) {
		chr = value.addr[i];

		if ((chr >= '0' && chr <= '9') ||
				(chr >= 'A' && chr <= 'F') ||
				(chr >= 'a' && chr <= 'f'))
			continue;
		return;
	}

	if ((color_ptr = malloc(8))) {
		memcpy(color_ptr, value.addr, 7);
		color_ptr[7] = '\0';
		*color->color = color_ptr;
	}
}

static inline void
updatecolors_size(Display *display, XrdbColor colors[],
		register unsigned int size)
{
	char *resm;
	register unsigned int i;
	XrmDatabase xrdb = NULL;

	if ((resm = XResourceManagerString(display)) &&
			(xrdb = XrmGetStringDatabase(resm)))

		for (i = 0; i < size; i++)
			xrdb_load_color(xrdb, &colors[i]);

	XrmDestroyDatabase(xrdb);
}
