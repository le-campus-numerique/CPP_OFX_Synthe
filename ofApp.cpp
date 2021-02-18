#include "ofApp.h"


float x_1 = 0;
float x_2 = 0;
float y_1 = 0;
float y_2 = 0;


float ofApp::filter(float x)
{
	float R = 1-3*0.0033;
	float ff = freq / FREQUENCE_ECH;
	float K = (1-2*R*cos(2*PI*ff)+R*R)/(2-2*cos(2*PI*ff));
	float a0 = 1-K;
	float a1 = 2*(K-R)*cos(2*PI*ff);
	float a2 = R*R-K;
	float b1 = 2*R*cos(2*PI*ff);
	float b2 = -(R*R);

	float y = a0*x + a1*x_1 + a2*x_2+b1*y_1+b2*y_2;

	x_2 = x_1;
	x_1 = x;
	y_2 = y_1;
	y_1 = y;
	return y;
}

/**
* Méthode appelée par la carte son toutes les float(nbEch)/FREQUENCE_ECH secondes (512. / 44100. = 11.6 millisecondes)
* signal est un pointeur vers un vecteur de nbEch échantillons de type flottant.
*
* Cette méthode doit stocker dans signal les échantillons temporels correspondant à un bloc de signal.
* Son temps d'exécution ne doit pas être suppérieur au temps d'appel par la carte son (ie 11.6 ms dans la configuration actuelle)
*/
void ofApp :: cbAudioProcess(float * s, int nbEch)
{
	// Calculer tous les échantillons du signal
	for (int i = 0; i < nbEch; i++)
	{
		// Calculer t
		t += 1 / FREQUENCE_ECH;

		// Caclculer le signal
		phi += phaseAdd;

		/*while (phi > 2 * PI)
			phi -= 2 * PI;*/

		s[i] = 0;
		if (formeOnde==0)
		{
			for (int k = 1; k <= brillance; k++)
			{
				s[i] += pow(-1, k) * sin(k*phi) / float(k);
			}
			s[i] *= volume;
		}
		else if (formeOnde==1)
		{
			s[i] = volume*ofRandomf();
			s[i] = filter(s[i]);
		}

	}
}


void ofApp :: cbFT(float * s, float * sFFT, int nbEch)
{
	for (int k = 0; k < nbEch; k++)
	{
		float real = 0;
		float imag = 0;
		for (int n = 0; n < nbEch; n++)
		{
			float theta = -2 * PI*k*n / float(nbEch);
			real = real + s[n] * cos(theta);
			imag = imag + s[n] * sin(theta);
		}
		sFFT[k] = sqrt(real*real + imag*imag);
	}
}



//--------------------------------------------------------------
void ofApp::setup(){

	ofBackground(34, 34, 34);

	// http://openframeworks.cc/documentation/sound/ofSoundStream/#show_setup
	soundStream.setup(this, 1, 0, FREQUENCE_ECH, TAILLE_BUFFER, 4);

	ofSetFrameRate(60);

    mouseX = 0;
    mouseY = 0;

	volume = 0.3;                     //< L'amplitude des signaux (dans l'inerval [0 : 1]
	freq = 440.;                      //< La fréquence du signal
	formeOnde = 0;                    //< Paramètre de forme d'onde
	brillance = 1;                    //< Paramètre de brillance
	t = 0;
	phi = 0;
	deltaPhi = 0;
	DeltaT = 1 / FREQUENCE_ECH;
}


//--------------------------------------------------------------
void ofApp::update(){

}

// Dessine le signal / Sa fft et affiche les données à l'ecran
//--------------------------------------------------------------
void ofApp::draw(){

	ofSetColor(225);
	ofDrawBitmapString("TP Theremin", 32, 32);

	ofNoFill();

	ofPushStyle();
		ofPushMatrix();
		ofTranslate(32, 150, 0);

		ofSetColor(225);
		ofDrawBitmapString("Domaine temporel", 4, 18);

		ofSetLineWidth(1);
		ofRect(0, 0, 900, 200);

		ofSetColor(245, 58, 135);
		ofSetLineWidth(3);

        ofBeginShape();
            for (unsigned int i = 0; i < TAILLE_BUFFER; i++)
            {
                float x =  ofMap(i, 0, TAILLE_BUFFER, 0, 900, true);
                float sample = signal[i];
                if (signal[i]>1.) signal[i] = 1.f;
                if (signal[i]<-1.) signal[i] = -1.f;
                ofVertex(x, 100.f -sample*100.0f);
            }
        ofEndShape(false);
		ofPopMatrix();
	ofPopStyle();

    ofPushStyle();
		ofPushMatrix();
		ofTranslate(32, 350, 0);

		ofSetColor(225);
		ofDrawBitmapString("Domaine fréquentiel", 4, 18);

		ofSetLineWidth(1);
		ofRect(0, 0, 900, 200);

		ofSetColor(245, 58, 135);
		ofSetLineWidth(3);

		cbFT(signal, sigFT, TAILLE_BUFFER);

        ofBeginShape();
            for (unsigned int i = 0; i < TAILLE_BUFFER/2; i++)
            {
                float x =  ofMap(i, 0, TAILLE_BUFFER/2, 0, 900, true);
                ofVertex(x, 200.f - sigFT[i]*0.5f);
            }
        ofEndShape(false);

		ofPopMatrix();
	ofPopStyle();

	ofSetColor(225);
	string reportString = "Volume : "+ofToString(volume, 2)+" \n";
	reportString += "Frequence : "+ofToString(freq, 2)+" hz \n";
    reportString += "Forme d'onde : " + ofToString(formeOnde, 2) + "\n";
    reportString += "Brillance : " + ofToString(brillance, 2) + "\n";
	reportString += "Position souris : ["+ofToString(mouseX, 2)+","+ofToString(mouseY, 2)+"]  \n";

	ofDrawBitmapString(reportString, 32, 579);

}




void ofApp::audioOut(float * sig, int nbEch, int nChannels)
{
	cbAudioProcess(sig, nbEch);
	memcpy(&signal[0], sig, nbEch * sizeof(int));
	for (unsigned int i = 0; i < TAILLE_BUFFER; i++)
	{
		if (signal[i]>1.) signal[i] = 1.f;
		if (signal[i]<-1.) signal[i] = -1.f;
	}

}



/**
* Méthode appelée par l'interface graphique lorsqu'une touche du clavier est enfoncée.
* Son paramètre est la lettre correspondant à la touche pressée (attention, key est sensible à la case).
*
* Cette méthode peut être utilisée pour créer des notes.
*/
void ofApp::keyPressed  (int key)
{

	if( key == 's' )
    {
		soundStream.start();
	}

	if( key == 'e' )
    {
		soundStream.stop();
	}

	if (key == '+')
	{
		brillance++;
	}
	if (key == '-')
	{
		brillance--;
	}
	if (key == 'o')
	{
		formeOnde++;
	}
	if (key == 'p')
	{
		formeOnde--;
	}
}

/**
* Méthode appelée par l'interface graphique lorsqu'une touche du clavier est relâchée.
* Son paramètre est la lettre correspondant à la touche relâchée (attention, key est sensible à la case).
*
* Cette méthode peut être utilisée pour stoper des notes.
*/
void ofApp::keyReleased  (int key)
{
}

/**
* Méthode appelée par l'interface graphique à chaque déplacement de la souris dans la fenêtre du programme.
* L'interface graphique appelle la méthode en passant la position de la souris (x et y) et la taille de la fenêtre (ofGetWidth et ofGetHeight).
* Cette méthode peut être utilisée pour contrôler la fréquence du synthétiseur et/ou le volume de celui-ci.
*/

void ofApp::mouseMoved(int x, int y )
{
	freq = x / float(ofGetWidth())*2000;
	volume = y / float(ofGetHeight());
	phaseAdd = 2 * PI*freq*DeltaT;
    mouseX = x;
    mouseY = y;
}

/**
* Méthode appelée par l'interface graphique à chaque déplacement de la souris dans la fenêtre du programme lorsqu'un boutton est enfoncé.
* L'interface graphique appelle la méthode en passant la position de la souris (x et y) et la taille de la fenêtre (windowWidth et windowHeight).
* Cette méthode peut être utilisée pour contrôler la brillance et la forme d'onde de celui-ci.
*/
void ofApp::mouseDragged(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{

}


//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg)
{

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo)
{

}
