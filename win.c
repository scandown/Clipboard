#include <windows.h>
#include <stdio.h>



int get_png(char **new_data);
int get_text(char **new_data);

int paste_clipboard(char **new_data, char *file_format_opt) {

	if (strlen(file_format_opt) == strlen("image")) {
		if (strncmp(file_format_opt, "image", strlen("image")) == 0) {
			get_png(new_data);
		}
	} else if (strlen(file_format_opt) == strlen("text")) {
		if (strncmp(file_format_opt, "text", strlen("text")) == 0) {
			get_text(new_data);
		}
	} else {
		fprintf(stderr, "Error: invalid file format given\n");
		return -1;
	}

	return 0;
}

int main(int argc, char **argv) {
	char *data;
	if (argc > 1) {
		paste_clipboard(&data, argv[1]);
	}

	printf("%s\n", data);

	free(data);
	return 0;
}

int get_png(char **new_data) {
	if (!OpenClipboard(NULL)) {
		return 1;
	}
	UINT PngFormat = RegisterClipboardFormat("PNG");

	printf("%d\n", PngFormat);

	HANDLE hdata = GetClipboardData(PngFormat);
	if (hdata == NULL) {
		CloseClipboard();
		return 1;
	}


	void *data = GlobalLock(hdata);
	char *mybuffer;
	if (data) {
		SIZE_T size = GlobalSize(hdata);

		mybuffer = malloc(size);
		memcpy(mybuffer, data, size);
		FILE *f = fopen("output.png", "wb");
		if (f) {
			fwrite(data, 1, size, f);   // writes 'size' bytes from 'data'
			fclose(f);
		} else {
			perror("Failed to open file");
		}
	}
	GlobalUnlock(hdata);


	CloseClipboard();
	*new_data = mybuffer;
	return 0;
}

int get_text(char **new_data) {
	if (!OpenClipboard(NULL)) {
		return 1;
	}
	HANDLE hdata = GetClipboardData(CF_TEXT);
	if (hdata == NULL) {
		CloseClipboard();
		return 1;
	}
	void *data = GlobalLock(hdata);
	char *mybuffer;
	if (data) {
		SIZE_T size = GlobalSize(hdata);

		mybuffer = malloc(size);
		memcpy(mybuffer, data, size);
		FILE *f = fopen("output.txt", "wb");
		if (f) {
			fwrite(data, 1, size, f);   // writes 'size' bytes from 'data'
			fclose(f);
		} else {
			perror("Failed to open file");
		}
	}
	GlobalUnlock(hdata);
	CloseClipboard();

	*new_data = mybuffer;
	return 0;
}
