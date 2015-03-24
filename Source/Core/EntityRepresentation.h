#pragma once

// using const char for POD for quick initialization
struct EntityRepresentation {
	const char *imageFile;
	Recti source;
};
