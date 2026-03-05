#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

int clipboard_paste(char *file_format_opt) {
	Display *display = XOpenDisplay(NULL);
	if (!display) {
		fprintf(stderr, "Cannot open X display\n");
		return 1;
	}

	Window window = XCreateSimpleWindow(display, RootWindow(display, DefaultScreen(display)),0, 0, 1, 1, 0, 0, 0);
	Atom clipboard = XInternAtom(display, "CLIPBOARD", False);
	Atom targets = XInternAtom(display, "TARGETS", False);
	Atom image_png = XInternAtom(display, "image/png", False);
	Atom text_plain = XInternAtom(display, "text/plain", False);
	Atom property = XInternAtom(display, "XSEL_DATA", False);
	Atom type_ret;
	int format_ret;
	unsigned long nitems_ret, bytes_after_ret;
	unsigned char *data_ret = NULL;

	// Request the current clipboard owner to convert to image/png
	
	if (strlen(file_format_opt) == strlen("image")) {
		if (strncmp(file_format_opt, "image", strlen("image")) == 0) {
			XConvertSelection(display, clipboard, image_png, property, window, CurrentTime);
		}
	} else if (strlen(file_format_opt) == strlen("text")) {
		if (strncmp(file_format_opt, "text", strlen("text")) == 0) {
			XConvertSelection(display, clipboard, text_plain, property, window, CurrentTime);
		}
	} else {
		fprintf(stderr, "Error: invalid file format given\n");
		return -1;
	}

	XSync(display, False);

	XEvent event;

	if (!XNextEvent(display, &event)) {
		if (event.type == SelectionNotify) {
			if (event.xselection.property == None) {
				fprintf(stderr, "Clipboard doesn't contain requested data\n");
				XFree(data_ret);
				XDestroyWindow(display, window);
				XCloseDisplay(display);
				return -1;
			}

			// Get the data from the property
			XGetWindowProperty(display, window, property, 0, 0x1FFFFFFF, True,
					AnyPropertyType, &type_ret, &format_ret,
					&nitems_ret, &bytes_after_ret, &data_ret);

			if (data_ret && nitems_ret > 0) {
				fwrite(data_ret, 1, nitems_ret, stdout);
			}

			XFree(data_ret);
		}
	}

	XDestroyWindow(display, window);
	XCloseDisplay(display);
	return 0;
}

int main(int argc, char **argv) {
	if (argc > 1) {
		clipboard_paste(argv[1]);
	}
	return 0;

}
