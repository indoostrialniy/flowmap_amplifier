/*
	2026-03-18  indoostrialniy  <pawel.iv.96@gmail.com>
 
	Flowmap amplifier v1.0
	
	Usage:
	Drag`n`drop an image onto the program shortcut and type reqiured amplifying value, the image will be rewrited.
	
	Goals:
	Sometimes it might be useful when you experiment with flowmap technic. 
	This program helps to change the velocity vectors encoded as color components quickly by just multiplying them on a specified coinstant.
	
	Warning!
	Currently it works correct only with PNG-images!

*/

#include <iostream>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_write.h"

#define SHOW(x) std::cout << (#x) << " : " << x << "\n";

int matAddImage(std::string textureName, float Ampl);

int main(int argc, char* argv[])
{
	std::cout << "Flowmap amplifier v1.0\n";
	
	if(argc != 2)
	{
		std::cerr << "No file selected! Click on your image and drag it to the programm icon\n"; 
		return 1;
	}
	
	try
	{
		float scal = 1.0f;
		std::cout << argv[1] << "\nInput scalar amplifier: ";
		std::cin >> scal;
		matAddImage( argv[1], scal );
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
	
	system("pause");
	return 0;
}

int matAddImage(std::string textureName, float Ampl)
{
	int width = 0;
	int height = 0;
	int channels = 0;
	
	unsigned char* data = stbi_load(textureName.c_str(), &width, &height, &channels, 0);
	stbi_image_free(data);

	data = stbi_load(textureName.c_str(), &width, &height, &channels, channels);
	
	std::cout << "read data " << textureName.c_str() << " \n";
	
	SHOW(width)
	SHOW(height)
	SHOW(channels)
	
	// корректируем данные
	for(int i = 0; i < width*height; i++ )
	{
		// texture coordinates are 0...255 (middle is 127), world coordinates meanwhile are -1...1 (middle is 0)
		
		// if red == 127 and green == 127  +- 2, then this is a zero-area (mask)	
		if( int(data[i*channels+0]) - 128 < 1 && int(data[i*channels+1]) - 128 < 1 ) // correct! Red Green
		{
			// no changes in mask!
			//data[i*channels+0] = data[i*channels+0]; // red
			//data[i*channels+1] = data[i*channels+1]; // green
			//data[i*channels+2] = data[i*channels+2]; // blue			
		}
		else // if value is non-128, then this is velocity map that needs correction
		{
			//~ if(i<10) std::cout << "\ni = " << i << std::endl;
			
			float red 		= data[i*channels+0];
			float green 	= data[i*channels+1];
			float blue 		= data[i*channels+2];
			
			// convert to world-space
			red 	= (red/255 - 0.5)*2;	// 0
			green 	= (green/255 - 0.5)*2;
			blue 	= (blue/255 - 0.5)*2;
					
			// amplify	
			float scalar = Ampl;
			red *= scalar;
			green *= scalar;
			blue *= scalar;
			
			// truncate
			if(red > 1) red = 1;
			if(red <-1) red = -1;
			if(green > 1) green = 1;
			if(green <-1) green = -1;
			if(blue > 1) blue = 1;
			if(blue <-1) blue = -1;
					
			// convert back to texture-space
			red 	= red*0.5 +0.5;
			green 	= green*0.5 +0.5;
			blue 	= blue*0.5 +0.5;
			red *= 255;
			green *= 255;
			blue *= 255;
			
			//rewrite
			data[i*channels+0] = red; 	
			data[i*channels+1] = green;	
			data[i*channels+2] = blue;	
		}
		
	}


	if( stbi_write_png(textureName.c_str(),	width, height, channels, data, width*channels) )
	{
		std::cout << "Image " << textureName << " corrected successfull!\n";
		stbi_image_free(data);
		return 0;
	}
	else
	{
		std::cout << "Error occured!\n";
		stbi_image_free(data);
		return -1;
	}
}
