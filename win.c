#include <windows.h>
#include <stdio.h>



int set_png(char *newdata, size_t newdata_size);
int get_png(char **new_data, size_t *new_size);
int get_text(char **new_data, size_t *new_size);

int paste_clipboard(char **new_data, char *file_format_opt, size_t *size) {

	if (strlen(file_format_opt) == strlen("image")) {
		if (strncmp(file_format_opt, "image", strlen("image")) == 0) {
			get_png(new_data, size);
		}
	} else if (strlen(file_format_opt) == strlen("text")) {
		if (strncmp(file_format_opt, "text", strlen("text")) == 0) {
			get_text(new_data, size);
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
		FILE *file = fopen(argv[1], "rb");
		if (file == NULL) {
			fprintf(stderr, "Couldn't open file\n");
			return 1;
		}

		char *buffer = NULL; // initialize buffer to NULL
		int buffer_size = 0;
		/*Open the file here*/
		// Read file character by character
		int c, i = 0;
		while ((c = fgetc(file)) != EOF) {
			// If buffer is full, resize it
			if (i >= buffer_size) {
				buffer_size += 1000; // increase buffer size by 1000 bytes
				buffer = realloc(buffer, buffer_size); // resize buffer
				if (buffer == NULL) {
					printf("Error: Memory allocation failed.\n");
					return 1;
				}
			}
			buffer[i] = c;
			i++;
		}

		set_png(buffer, buffer_size);
		free(buffer);
	}

	free(data);
	return 0;
}

int set_png(char *newdata, size_t newdata_size) {
	if (!OpenClipboard(NULL)) {
		return 1;
	}

	UINT PngFormat = RegisterClipboardFormat("PNG");

	HANDLE hdata = GlobalAlloc(GMEM_MOVEABLE, newdata_size);
	if (hdata == NULL) {
		CloseClipboard();
		return 1;
	}

	if (!EmptyClipboard()) {
		CloseClipboard();
		return 1;
	}


	void *data = GlobalLock(hdata);
	if (data == NULL) {
		CloseClipboard();
		return 1;
	}

	memcpy(data, newdata, newdata_size);
	if (SetClipboardData(PngFormat, hdata) == NULL) {
		CloseClipboard();
		GlobalFree(hdata);
		return 1;
	}

	GlobalUnlock(hdata);
	CloseClipboard();
	return 0;
}

int get_png(char **new_data, size_t *new_size) {
	if (!OpenClipboard(NULL)) {
		return 1;
	}
	UINT PngFormat = RegisterClipboardFormat("PNG");

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
		*new_size = size;
	}
	GlobalUnlock(hdata);


	CloseClipboard();
	*new_data = mybuffer;
	return 0;
}

int get_text(char **new_data, size_t *new_size) {
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
		*new_size = size;
	}
	GlobalUnlock(hdata);
	CloseClipboard();

	*new_data = mybuffer;
	return 0;
}
