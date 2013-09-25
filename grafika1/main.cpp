//=============================================================================================
// Szamitogepes grafika hazi feladat keret. Ervenyes 2013-tol.
// A //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// sorokon beluli reszben celszeru garazdalkodni, mert a tobbit ugyis toroljuk.
// A beadott program csak ebben a fajlban lehet, a fajl 1 byte-os ASCII karaktereket tartalmazhat.
// Tilos:
// - mast "beincludolni", illetve mas konyvtarat hasznalni
// - faljmuveleteket vegezni (printf is fajlmuvelet!)
// - new operatort hivni az onInitialization függvényt kivéve, a lefoglalt adat korrekt felszabadítása nélkül
// - felesleges programsorokat a beadott programban hagyni
// - tovabbi kommenteket a beadott programba irni a forrasmegjelolest kommentjeit kiveve
// ---------------------------------------------------------------------------------------------
// A feladatot ANSI C++ nyelvu forditoprogrammal ellenorizzuk, a Visual Studio-hoz kepesti elteresekrol
// es a leggyakoribb hibakrol (pl. ideiglenes objektumot nem lehet referencia tipusnak ertekul adni)
// a hazibeado portal ad egy osszefoglalot.
// ---------------------------------------------------------------------------------------------
// A feladatmegoldasokban csak olyan gl/glu/glut fuggvenyek hasznalhatok, amelyek
// 1. Az oran a feladatkiadasig elhangzottak ES (logikai AND muvelet)
// 2. Az alabbi listaban szerepelnek:
// Rendering pass: glBegin, glVertex[2|3]f, glColor3f, glNormal3f, glTexCoord2f, glEnd, glDrawPixels
// Transzformaciok: glViewport, glMatrixMode, glLoadIdentity, glMultMatrixf, gluOrtho2D,
// glTranslatef, glRotatef, glScalef, gluLookAt, gluPerspective, glPushMatrix, glPopMatrix,
// Illuminacio: glMaterialfv, glMaterialfv, glMaterialf, glLightfv
// Texturazas: glGenTextures, glBindTexture, glTexParameteri, glTexImage2D, glTexEnvi,
// Pipeline vezerles: glShadeModel, glEnable/Disable a kovetkezokre:
// GL_LIGHTING, GL_NORMALIZE, GL_DEPTH_TEST, GL_CULL_FACE, GL_TEXTURE_2D, GL_BLEND, GL_LIGHT[0..7]
//
// NYILATKOZAT
// ---------------------------------------------------------------------------------------------
// Nev    : <VEZETEKNEV(EK)> <KERESZTNEV(EK)>
// Neptun : <NEPTUN KOD>
// ---------------------------------------------------------------------------------------------
// ezennel kijelentem, hogy a feladatot magam keszitettem, es ha barmilyen segitseget igenybe vettem vagy
// mas szellemi termeket felhasznaltam, akkor a forrast es az atvett reszt kommentekben egyertelmuen jeloltem.
// A forrasmegjeloles kotelme vonatkozik az eloadas foliakat es a targy oktatoi, illetve a
// grafhazi doktor tanacsait kiveve barmilyen csatornan (szoban, irasban, Interneten, stb.) erkezo minden egyeb
// informaciora (keplet, program, algoritmus, stb.). Kijelentem, hogy a forrasmegjelolessel atvett reszeket is ertem,
// azok helyessegere matematikai bizonyitast tudok adni. Tisztaban vagyok azzal, hogy az atvett reszek nem szamitanak
// a sajat kontribucioba, igy a feladat elfogadasarol a tobbi resz mennyisege es minosege alapjan szuletik dontes.
// Tudomasul veszem, hogy a forrasmegjeloles kotelmenek megsertese eseten a hazifeladatra adhato pontokat
// negativ elojellel szamoljak el es ezzel parhuzamosan eljaras is indul velem szemben.
//=============================================================================================

#include <math.h>
#include <stdlib.h>

#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Innentol modosithatod...

//--------------------------------------------------------
// 3D Vektor
//--------------------------------------------------------
struct Vector {
    float x, y, z;
    
    Vector( ) {
        x = y = z = 0;
    }
    Vector(float x0, float y0, float z0 = 0) {
        x = x0; y = y0; z = z0;
    }
    Vector operator*(float a) {
        return Vector(x * a, y * a, z * a);
    }
    Vector operator+(const Vector& v) {
        return Vector(x + v.x, y + v.y, z + v.z);
    }
    Vector operator-(const Vector& v) {
        return Vector(x - v.x, y - v.y, z - v.z);
    }
    float operator*(const Vector& v) { 	// dot product
        return (x * v.x + y * v.y + z * v.z);
    }
    Vector operator%(const Vector& v) { 	// cross product
        return Vector(y*v.z-z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
    }
    float Length() { return sqrt(x * x + y * y + z * z); }
};

//--------------------------------------------------------
// Spektrum illetve szin
//--------------------------------------------------------
struct Color {
    float r, g, b;
    
    Color( ) {
        r = g = b = 0;
    }
    Color(float r0, float g0, float b0) {
        r = r0; g = g0; b = b0;
    }
    Color operator*(float a) {
        return Color(r * a, g * a, b * a);
    }
    Color operator*(const Color& c) {
        return Color(r * c.r, g * c.g, b * c.b);
    }
    Color operator+(const Color& c) {
        return Color(r + c.r, g + c.g, b + c.b);
    }
};

const int screenWidth = 600;	// alkalmazás ablak felbontása
const int screenHeight = 600;


Color image[screenWidth*screenHeight];	// egy alkalmazás ablaknyi kép


// Inicializacio, a program futasanak kezdeten, az OpenGL kontextus letrehozasa utan hivodik meg (ld. main() fv.)
void onInitialization( ) {
	glViewport(0, 0, screenWidth, screenHeight);
    
    
    // Peldakent keszitunk egy kepet az operativ memoriaba
    for(int Y = 0; Y < screenHeight; Y++)
		for(int X = 0; X < screenWidth; X++)
		image[Y*screenWidth + X] = Color(0.12,0.45,1);
    
}

float birdPositionX = 0;
float birdPositionY = 0;
long time_slow = 0;

class Bird{
    
    double x;
    double y;
    
public:
    Bird();
    
    void drawBird(double x, double y, float R, float G, float B){
        
        for(int i = 0; i < 360; i++){
            float rad = i / 180.0 * 3.14;
            glVertex2f(cos(rad) * 0.1, sin(rad) * 0.1 - 1);
        }
        glEnd();
    
    }
        
    double getGreenBirdYForTime(float time){
        float rad = time / 180.0 * 3.14;
        
        return sin(rad);
    }
        
    double getGreenBirdXForTime(float time){
        return time;
    }
        
        
};


double trans_x(float x){
    float a = 1.0/3.0 + 0.1/3.0;
    x = -(1 - (x+a));
    
return x;
}

double trans_y(float y){
    float b = 0.1/3.0;
    y = -(1 - y + b + 0.05);
    
return y;
}


void drawBird(float c, float x, float y, float R, float G, float B){
    
    
    
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(R, G, B);
    
    
    
    for(int i = 0; i < 360; i++){
        float rad = i / 180.0 * 3.14;
        glVertex2f(cos(rad) * 0.08 + c * x, sin(rad) * 0.1 + y);
    }
    glEnd();
    
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(1, 1, 1);
    
    
    
    for(int i = 0; i < 360; i++){
        float rad = i / 180.0 * 3.14;
        glVertex2f(cos(rad) * 0.033 + c * (0.036 + x), sin(rad) * 0.05 + 0.02 + y);
    }
    glEnd();
    
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(0.0, 0.0, 0.0);
    
    
    
    for(int i = 0; i < 360; i++){
        float rad = i / 180.0 * 3.14;
        glVertex2f(cos(rad) * 0.015 + c * (0.05 + x), sin(rad) * 0.025 + 0.01 + y);
    }
    glEnd();
    
    
    glBegin(GL_TRIANGLES);
    glColor3f(0.85, 0.75, 0.2);
    
    glVertex2f(c * (0.12 + x), -0.05 + y);
    glVertex2f(c * (0.06 + x), -0.07 + y);
    glVertex2f(c * (0.08 + x), -0.005 + y);
    
    glEnd();
    
    glBegin(GL_TRIANGLES);
    glColor3f(0.0, 0.0, 0.0);
    
    glVertex2f(c * (0.09 + x), 0.04 + y);
    glVertex2f(c * (0.04 + x), 0.11 + y);
    glVertex2f(c * (-0.02 + x), 0.13 + y);
    
    glEnd();
    
    
    glBegin(GL_TRIANGLES);
    glColor3f(1.0, 1.0, 0.0);
    
    glVertex2f(c * (-0.06 + x), -0.06 + y);
    glVertex2f(c * (-0.12 + x), -0.02 + y);
    glVertex2f(c * (-0.14 + x), -0.03 + y);
    
    glEnd();
    
    glBegin(GL_TRIANGLES);
    glColor3f(1.0, 1.0, 0.0);
    
    glVertex2f(c * (-0.06 + x), -0.06 + y);
    glVertex2f(c * (-0.11 + x), -0.07 + y);
    glVertex2f(c * (-0.13 + x), -0.05 + y);
    
    glEnd();
    
    glBegin(GL_TRIANGLES);
    glColor3f(1.0, 1.0, 0.0);
    
    glVertex2f(c * (-0.06 + x), -0.06 + y);
    glVertex2f(c * (-0.1 + x), -0.1 + y);
    glVertex2f(c * (-0.1 + x), -0.08 + y);
    
    glEnd();
    
    glBegin(GL_TRIANGLES);
    glColor3f(0.85, 0.75, 0.2);
    
    glVertex2f(c * (-0.07 + x), 0.0 + y);
    glVertex2f(c * (-0.02 + x), -0.07 + y);
    glVertex2f(c * (0.02 + x), -0.05 + y);
    
    glEnd();
    
}

void getGreenBirdYForTime(float time){

    float rad = time / 180.0 * 3.14;
    
    birdPositionY = sin(rad)/2.0;
    
}


// Rajzolas, ha az alkalmazas ablak ervenytelenne valik, akkor ez a fuggveny hivodik meg
void onDisplay( ) {
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);		// torlesi szin beallitasa
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // kepernyo torles
    
    // ..
    
    
    // Peldakent atmasoljuk a kepet a rasztertarba
    glDrawPixels(screenWidth, screenHeight, GL_RGB, GL_FLOAT, image);
    // Majd rajzolunk egy kek haromszoget
	//glColor3f(0, 0, 1);
	//glBegin(GL_TRIANGLES);
    //glVertex2f(-1.5f, -0.5f);
    //glVertex2f( 0.5f, -0.5f);
    //glVertex2f( 0.0f,  0.5f);
    
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(0, 1, 0.3);
    
    for(int i=0;i<360;i++){
        float rad = i/180.0*3.14;
        glVertex2f(cos(rad)*1.5, sin(rad)*0.6-1.3);
    }
    
    glEnd();
    

    
    glBegin(GL_POLYGON);
    glColor3f(0.7, 0.5, 0.25);
    
    glVertex2f( trans_x(0.225), trans_y(0.12));
    glVertex2f( trans_x(0.375), trans_y(0.12));
    glVertex2f( trans_x(0.375), trans_y(0.5));    // 1. szár
    glVertex2f( trans_x(0.6), trans_y(0.8));      // 1. oldalág
    glVertex2f( trans_x(0.55), trans_y(0.8));
    glVertex2f( trans_x(0.3), trans_y(0.45));     // középpont
        
    glEnd();
    
    glBegin(GL_POLYGON);
    glColor3f(0.7, 0.5, 0.25);
    
    glVertex2f( trans_x(0.375), trans_y(0.12));
    glVertex2f( trans_x(0.225), trans_y(0.12));         // 2. szár
    glVertex2f( trans_x(0.225), trans_y(0.5));
    glVertex2f( trans_x(0.3), trans_y(0.45));
    glVertex2f( trans_x(0.05), trans_y(0.8));           // 2. oldalág
    glVertex2f( trans_x(0.0), trans_y(0.8));
    
    glEnd();
    
    
    drawBird(-1.0, -0.7, birdPositionY, 0.5, 1.0, 0.2);
    drawBird(1.0, -0.33, -0.25, 1.0, 0.0, 0.0);
    
    glPointSize(5.0);
    glBegin(GL_POINTS);
    glColor3d(1,0,0);
    glVertex2f(-1.0/3.0, -1.0/3.0);
    
    glEnd();
    
    // ... 
    
    glutSwapBuffers();     				// Buffercsere: rajzolas vege
    
}


// Billentyuzet esemenyeket lekezelo fuggveny (lenyomas)
void onKeyboard(unsigned char key, int x, int y) {
    if (key == 'd') glutPostRedisplay( ); 		// d beture rajzold ujra a kepet
    
}

// Billentyuzet esemenyeket lekezelo fuggveny (felengedes)
void onKeyboardUp(unsigned char key, int x, int y) {
    
}

// Eger esemenyeket lekezelo fuggveny
void onMouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)   // A GLUT_LEFT_BUTTON / GLUT_RIGHT_BUTTON illetve GLUT_DOWN / GLUT_UP
        
        glutPostRedisplay( );           // Ilyenkor rajzold ujra a kepet
}

// Eger mozgast lekezelo fuggveny
void onMouseMotion(int x, int y){
    
}


// `Idle' esemenykezelo, jelzi, hogy az ido telik, az Idle esemenyek frekvenciajara csak a 0 a garantalt minimalis ertek
void onIdle( ){
    long time = glutGet(GLUT_ELAPSED_TIME);     // program inditasa ota eltelt ido
    
    if(time % 1 == 0) time_slow++;

    
    getGreenBirdYForTime(time_slow);
    
    glutPostRedisplay( );
  
}

// ...Idaig modosithatod
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// A C++ program belepesi pontja, a main fuggvenyt mar nem szabad bantani
int main(int argc, char **argv) {
    glutInit(&argc, argv); 				// GLUT inicializalasa
    glutInitWindowSize(600, 600);			// Alkalmazas ablak kezdeti merete 600x600 pixel
    glutInitWindowPosition(100, 100);			// Az elozo alkalmazas ablakhoz kepest hol tunik fel
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);	// 8 bites R,G,B,A + dupla buffer + melyseg buffer
    
    glutCreateWindow("Grafika hazi feladat");		// Alkalmazas ablak megszuletik es megjelenik a kepernyon
    
    glMatrixMode(GL_MODELVIEW);				// A MODELVIEW transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);			// A PROJECTION transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();
    
    onInitialization();					// Az altalad irt inicializalast lefuttatjuk
    
    glutDisplayFunc(onDisplay);				// Esemenykezelok regisztralasa
    glutMouseFunc(onMouse);
    glutIdleFunc(onIdle);
    glutKeyboardFunc(onKeyboard);
    glutKeyboardUpFunc(onKeyboardUp);
    glutMotionFunc(onMouseMotion);
    
    glutMainLoop();					// Esemenykezelo hurok
    
    return 0;
}

