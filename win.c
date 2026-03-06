#include <windows.h>
#include <stdio.h>

int paste_clipboard(char **new_data) {
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

int main() {
	char *data;
	paste_clipboard(&data);

	printf("%s\n", data);

	free(data);
	return 0;
}
