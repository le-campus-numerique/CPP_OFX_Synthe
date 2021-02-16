#pragma once

#include "ofMain.h"


class ofApp : public ofBaseApp{


private : 
	const float FREQUENCE_ECH = 44100.;     //< La fréquence d'échantillonnage utilisée par la carte son.
	float volume = 0.3;                     //< L'amplitude des signaux (dans l'inerval [0 : 1]
	float freq = 440.;                      //< La fréquence du signal
	int formeOnde = 0;                      //< Paramètre de forme d'onde
	int brillance = 1;                      //< Paramètre de brillance
	float t = 0;
	float phi = 0;
	float deltaPhi = 0;
	float DeltaT = 1 / FREQUENCE_ECH;



public:
    ofApp() : TAILLE_BUFFER(512)
    {
		signal = new float[TAILLE_BUFFER];
		sigFT = new float[TAILLE_BUFFER];
    }
    ~ofApp()
    {
		delete[] signal;
		delete[] sigFT;
    }
	void setup();
	void update();
	void draw();

	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	void audioOut(float * input, int bufferSize, int nChannels);

	void cbAudioProcess(float * s, int nbEch);
	void cbFT(float * s, float * sFT, int nbEch);

	ofSoundStream soundStream;

	int mouseX;
	int mouseY;

    int TAILLE_BUFFER;
	float *signal;
	float *sigFT;

};
