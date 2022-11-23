#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>

class STexture{
    public:
        STexture(SDL_Renderer* gRenderer, SDL_Window* gWindow);
        ~STexture();
        void load(std::string path);

        void render(int x, int y, int w = 0, int h = 0, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );

        
        void executeShader(Uint32(*func)(Uint32, float), float dT);
        void executeShaderPosition(Uint32(*func)(Uint32, float), float dT, int x1, int x2);
        Uint32 color(Uint8 red, Uint8 green, Uint8 blue);
        
        int getWidth();
        int getHeight();

        bool lockTexture();
		bool unlockTexture();
		Uint32* getPixels();
		int getPitch();
        int getPixelCount();
        Uint32 getPixel(int x, int y);
        Uint32 getPixel(int xy);

        void free();
    
    private:
        SDL_Window* window;
        SDL_Renderer* renderer;
        int tWidth;
        int tHeight;
        Uint32 format;
        SDL_PixelFormat* mappingFormat;

        void* mPixels;
        int mPitch;
        SDL_Texture* mTexture;
};

STexture::STexture(SDL_Renderer* gRenderer, SDL_Window* gWindow){
    renderer = gRenderer;
    window = gWindow;

    tHeight = 0;
    tWidth = 0;
    mPitch = 0;
    mPixels = NULL;
    mTexture = NULL;

    format = SDL_GetWindowPixelFormat(window);
    mappingFormat = SDL_AllocFormat(format);
}
STexture::~STexture(){
    SDL_FreeFormat(mappingFormat);
    SDL_DestroyTexture(mTexture);

    mTexture = NULL;
    tWidth = 0;
    tHeight = 0;
    mPixels = NULL;
    mPitch = 0;
}

void STexture::executeShader(Uint32(*func)(Uint32, float), float dT)
{
	lockTexture();
    int pixelCount = getPixelCount();
    Uint32* pixels = getPixels();
    
    for( int i = 0; i < pixelCount; ++i)
    {
        pixels[i] = func(pixels[i], dT);
    };
    
    unlockTexture();
}

void STexture::executeShaderPosition(Uint32(*func)(Uint32, float), float dT, int x1, int x2)
{
	lockTexture();
    int pixelCount = getPixelCount();
    Uint32* pixels = getPixels();
    
    for (int i = x1; i < x2; ++i){
        for(int j = 0; j < tHeight; ++j){

            pixels[(j * getPitch() / 4) + i] = func(pixels[(j * getPitch() / 4) + i], dT);
        }
    }
    
    
    unlockTexture();
}

void STexture::load(std::string path){
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat(loadedSurface, SDL_GetWindowPixelFormat(window), 0);

    SDL_Texture* newTexture = NULL;
    newTexture = SDL_CreateTexture(renderer, SDL_GetWindowPixelFormat(window), SDL_TEXTUREACCESS_STREAMING, formattedSurface->w, formattedSurface->h);

    SDL_LockTexture(newTexture, NULL, &mPixels, &mPitch);

    memcpy(
        mPixels,
        formattedSurface->pixels,
        formattedSurface->pitch * formattedSurface->h
    );


    tWidth = formattedSurface->w;
    tHeight = formattedSurface->h;
    mPixels = NULL;

    SDL_FreeSurface(loadedSurface);
    SDL_FreeSurface(formattedSurface);

    mTexture = newTexture;
}

Uint32 STexture::color(Uint8 red, Uint8 green, Uint8 blue)
{
    return SDL_MapRGB(mappingFormat, red, green, blue);
}

int STexture::getPitch()
{
    return mPitch;
}

void STexture::render(int x, int y, int w, int h, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip){
    int rWidth = tWidth;
    int rHeight = tHeight;

    if (w != 0) {
        rWidth = w;
    }

    if (h != 0) {
        rHeight = h;
    }

    SDL_Rect renderQuad = { x, y, rWidth, rHeight };

    if( clip != NULL )
    {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }

    SDL_RenderCopy(renderer, mTexture, NULL, &renderQuad);
}

int STexture::getWidth()
{
	return tWidth;
}

int STexture::getHeight()
{
	return tHeight;
}

bool STexture::lockTexture()
{
	if(mPixels != NULL)
	{
        return false;
	}

    SDL_LockTexture(mTexture, NULL, &mPixels, &mPitch);
	return true;
}

bool STexture::unlockTexture()
{
	if(mPixels == NULL)
	{
        return true;
	}

    SDL_UnlockTexture(mTexture);
    mPixels = NULL;
    mPitch = 0;

    return true;
}

Uint32* STexture::getPixels()
{
	return (Uint32*) mPixels;
}

int STexture::getPixelCount()
{
	return (getPitch() / 4) * getHeight();
}

Uint32 STexture::getPixel(int x, int y)
{
    lockTexture();
    Uint32* pixels = getPixels();
    Uint32 pixel = pixels[(y * getPitch() / 4) + x];
    unlockTexture();

	return pixel;
}

Uint32 STexture::getPixel(int xy)
{
  lockTexture();
  Uint32* pixels = getPixels();
  Uint32 pixel = pixels[xy];
  unlockTexture();

	return pixel;
}