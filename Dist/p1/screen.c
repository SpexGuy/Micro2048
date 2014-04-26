#include "screen.h"
#include <string.h>

void clearFrame(FrameBuffer *buffer) {
	memset(buffer, 0, sizeof(*buffer));
}
