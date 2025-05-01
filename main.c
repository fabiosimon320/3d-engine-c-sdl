#include <stdio.h>


#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"
#include "math.h"



struct vec3d{
    float x,y,z,w;

};

struct triangle{
    struct vec3d vertex[3];
};

struct mesh{
    struct triangle* object;
    int numTriangle;
};

struct plane{
    float a,b,c,d;

};

void proiezione(struct vec3d *p1, struct vec3d *p2);

void drawFunc(struct triangle tri, SDL_Renderer *renderer){

    struct triangle output;

    proiezione(&tri.vertex[0], &output.vertex[0]);
    proiezione(&tri.vertex[1], &output.vertex[1]);
    proiezione(&tri.vertex[2], &output.vertex[2]);





    // triangoli anti orari





    for (int j = 0; j < 3; ++j) { // 3 vertici per triangolo
        output.vertex[j].x = (output.vertex[j].x + 1) * 0.5f * 1600;


        output.vertex[j].y = (1 - output.vertex[j].y) * 0.5f * 800;


    }


    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

    SDL_RenderDrawLine(renderer, output.vertex[0].x, output.vertex[0].y,
                       output.vertex[1].x, output.vertex[1].y);

    SDL_SetRenderDrawColor(renderer, 255, 9, 0, 255);
    SDL_RenderDrawLine(renderer, output.vertex[1].x, output.vertex[1].y,
                       output.vertex[2].x, output.vertex[2].y);

    SDL_SetRenderDrawColor(renderer, 255, 0, 9, 255);
    SDL_RenderDrawLine(renderer, output.vertex[2].x, output.vertex[2].y,
                       output.vertex[0].x, output.vertex[0].y);



    //fillTriangle(renderer, CuboProj.object[i].vertex[0], CuboProj.object[i].vertex[1], CuboProj.object[i].vertex[2]);



}


struct vec3d intersectionPlane(struct plane p, struct vec3d p1, struct vec3d p2);

int clippingPlane(struct triangle tri, struct plane plane, struct triangle *array, int currenttriangle, int newtriangle){

    struct vec3d InsidePoint[3];
    int insidepoint = 0;

    struct vec3d OutsidePoint[3];
    int outsidepoint = 0;



    for (int i = 0; i < 3; ++i) {
        struct vec3d p = tri.vertex[i];
        if(p.x * plane.a + p.y *plane.b + p.z*plane.c >= plane.d){
            InsidePoint[insidepoint] = tri.vertex[i];
            insidepoint++;
        }else{
            OutsidePoint[outsidepoint] = tri.vertex[i];
            outsidepoint++;
        }

    }

    if(insidepoint == 3){
        array[currenttriangle] = tri;
        return 1;
    }
    if(outsidepoint == 3){

        return 0;
    }

    if(insidepoint == 1){
        tri.vertex[0] = InsidePoint[0];
        tri.vertex[1] = intersectionPlane(plane ,InsidePoint[0], OutsidePoint[0]);
        tri.vertex[2] = intersectionPlane(plane,InsidePoint[0], OutsidePoint[1]);

        array[currenttriangle] = tri;
        return 1;
    }

    if(insidepoint == 2){
        tri.vertex[0] = InsidePoint[0];
        tri.vertex[1] = intersectionPlane(plane,InsidePoint[0], OutsidePoint[0]);
        tri.vertex[2] = InsidePoint[1];

        struct triangle clip;



        clip.vertex[0] = InsidePoint[1];
        clip.vertex[1] = tri.vertex[1];
        clip.vertex[2] = intersectionPlane(plane, InsidePoint[1], OutsidePoint[0]);


        array[currenttriangle] = tri;
        array[newtriangle] = clip;

        return 2;
    }
}


struct vec3d intersectionPlane(struct plane p, struct vec3d p1, struct vec3d p2){

    float t = (p.d - (p.a* p1.x + p.b * p1.y + p.c * p1.z))/
            (p.a*(p2.x-p1.x) + p.b*(p2.y - p1.y) + p.c*(p2.z-p1.z));

    float x = p1.x + t * (p2.x-p1.x);
    float y = p1.y + t * (p2.y-p1.y);

    float z = p1.z + t * (p2.z-p1.z);


    return (struct vec3d){x,y,z, 1};



}


void crossProduct(struct vec3d*, struct vec3d, struct vec3d);

void normalize(struct vec3d *, struct vec3d);

float dot(struct vec3d v1, struct vec3d v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

struct vec3d intersectLinePlane(struct vec3d a, struct vec3d b, struct vec3d zNearPlane) {
    float t = (zNearPlane.z - a.z) / (b.z - a.z);
    struct vec3d result;
    result.x = a.x + t * (b.x - a.x);
    result.y = a.y + t * (b.y - a.y);
    result.z = zNearPlane.z;
    return result;
}


void viewMatrix(float matrice[4][4], struct vec3d *posCamera, struct vec3d *target, struct vec3d *up) {

    //Calcoliamo nuovo vettore forward

    struct vec3d newForward = {target->x - posCamera->x,
                               target->y - posCamera->y,
                               target->z - posCamera->z};
    //normalize
    normalize(&newForward, newForward);
    //printf("forward: %f,%f,%f\n", newForward.x, newForward.y, newForward.z);
    //Calcoliamo la nuova direzione Up
    float oldRight = up->x * newForward.x + up->y * newForward.y + up->z * newForward.z;

    struct vec3d newUp = {newForward.x * oldRight,
                          newForward.y * oldRight,
                          newForward.z * oldRight};

    newUp.x = up->x - newUp.x, newUp.y = up->y - newUp.y, newUp.z = up->z - newUp.z;
    normalize(&newUp, newUp);
    //printf("up: %f,%f,%f\n", newUp.x, newUp.y, newUp.z);

    struct vec3d newRight;
    crossProduct(&newRight, newUp, newForward);

    normalize(&newRight, newRight);

    //printf("%f,%f,%f\n", newRight.x, newRight.y, newRight.z);

    // Popola la matrice di vista (convenzione standard destrorsa)
    matrice[0][0] = newRight.x;
    matrice[0][1] = newRight.y;
    matrice[0][2] = newRight.z;
    matrice[0][3] = -dot(newRight, *posCamera); // Traslazione X

    matrice[1][0] = newUp.x;
    matrice[1][1] = newUp.y;
    matrice[1][2] = newUp.z;
    matrice[1][3] = -dot(newUp, *posCamera);    // Traslazione Y

    matrice[2][0] = newForward.x; // Nota il segno meno per Forward
    matrice[2][1] = newForward.y;
    matrice[2][2] = newForward.z;
    matrice[2][3] = -dot(newForward, *posCamera); // Traslazione Z (potrebbe essere +dot a seconda della convenzione Z)

    matrice[3][0] = 0;
    matrice[3][1] = 0;
    matrice[3][2] = 0;
    matrice[3][3] = 1;


}



struct vec3d Matrix_MultiplyDirection(float matrix[4][4], struct vec3d v) {
    struct vec3d result;

    v.w = 0;

    result.x = matrix[0][0] * v.x + matrix[1][0] * v.y + matrix[2][0] * v.z + matrix[3][0] * v.w;
    result.y = matrix[0][1] * v.x + matrix[1][1] * v.y + matrix[2][1] * v.z + matrix[3][1] * v.w;
    result.z = matrix[0][2] * v.x + matrix[1][2] * v.y + matrix[2][2] * v.z + matrix[3][2] * v.w;
    result.w = 0;

    return result;


}

struct vec3d MatrixMult(float matrix[4][4], struct vec3d v) {
    struct vec3d result;

    // Estendiamo il vettore a omogeneo (w = 1)
    v.w = 1.0f;

    // Moltiplichiamo la matrice per il vettore
    result.x = matrix[0][0] * v.x + matrix[0][1] * v.y + matrix[0][2] * v.z + matrix[0][3] * v.w;
    result.y = matrix[1][0] * v.x + matrix[1][1] * v.y + matrix[1][2] * v.z + matrix[1][3] * v.w;
    result.z = matrix[2][0] * v.x + matrix[2][1] * v.y + matrix[2][2] * v.z + matrix[2][3] * v.w;
    //result.w = matrix[3][0] * v.x + matrix[3][1] * v.y + matrix[3][2] * v.z + matrix[3][3] * v.w;

    return result;
}




void fillTriangle(SDL_Renderer *renderer, struct vec3d p1, struct vec3d p2, struct vec3d p3){

    struct vec3d point[3] = {p1,p2,p3};


    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 2; ++j) {

            if(point[j].y < point[j+1].y){
                struct vec3d swap = point[j+1];
                point[j+1] = point[j];
                point[j] = swap;
            }

        }
    }

    for (int h = (int)(point[0].y) -1; h >= (int)point[2].y  ; --h) {

        float diffy0y1= point[1].y - point[0].y;
        float diffy0y2= point[2].y - point[0].y;

        float diffx0x1= point[1].x - point[0].x;
        float diffx0x2= point[2].x - point[0].x;

        float diffy1y2 = point[2].y - point[1].y;
        float diffx1x2 = point[2].x - point[1].x;

        if(diffy0y2 < 0){ diffy0y2 = diffy0y2*-1; }
        if(diffy0y1 < 0){ diffy0y1 = diffy0y1*-1; }
        if(diffx0x2 < 0){ diffx0x2 = diffx0x2*-1; }
        if(diffx0x1 < 0){ diffx0x1 = diffx0x1*-1; }
        if(diffx1x2 < 0){ diffx1x2 = diffx1x2*-1; }
        if(diffy1y2 < 0){ diffy1y2 = diffy1y2*-1; }


        SDL_Rect rect;


        if( h >= (int) point[1].y){


            int height = point[0].y - h; // 50
            int x1;
            int x2;


            if(diffx0x1 == 0){
                x1 = point[0].x;

            }else{
                float increment = diffx0x1 / diffy0y1; // 160  / 100 = 1.6

                if(point[0].x > point[1].x){
                    x1 = point[0].x - (height * increment); //15
                }else{
                    x1 = point[0].x +( height * increment); //15
                }

            }

            if(diffx0x2 == 0){
                x2 = point[0].x;

            }else{

                float increment1 = diffx0x2 / diffy0y2; // 100/130

                if(point[0].x < point[2].x){
                    x2 = ( height * increment1) + point[0].x;
                }else{
                    x2 = point[0].x - ( height * increment1);
                }


            }

            if(x2 <= x1){
                rect.x = x2;
                rect.y = h;
                rect.h = 1;
                rect.w = x1-x2;
            }else{
                rect.x = x1;
                rect.y = h;
                rect.h = 1;
                rect.w = x2-x1;
            }
            SDL_RenderFillRect(renderer, &rect);

        }else{

            int height = point[1].y - h; // 20
            int totalHeight = point[0].y - h;
            int x1,x2;

            if(diffx0x2 == 0){
                x1 = point[0].x;

            }else{

                float increment1 = diffx0x2 / diffy0y2; //  20 / 50 =  0.4


                if(point[0].x < point[2].x){
                    x1 = point[0].x + ( totalHeight * increment1);
                }else{
                    x1 = point[0].x - ( totalHeight * increment1);
                }


            }

            if(diffx1x2 == 0){

                x2 = point[2].x;

            }else{
                float increment = diffx1x2 / diffy1y2; // -5 / 10 = -0,5

                if(point[1].x > point[2].x){
                    x2 = point[1].x - ( height * increment); //15
                }else{
                    x2 = point[1].x + ( height * increment); //15
                }



            }

            if(x2 <= x1){
                rect.x = x2;
                rect.y = h;
                rect.h = 1;
                rect.w = x1-x2;    // X2 COINCIDE CON LA DIAGONALE
            }else{
                rect.x = x1;
                rect.y = h;
                rect.h = 1;
                rect.w = x2-x1;


            }

            SDL_RenderFillRect(renderer, &rect);




        }

    }

}

void normalize(struct vec3d *normalizeVector, struct vec3d vector){

    float lenght = sqrtf(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);

    if(lenght == 0){
        printf("errore");

    }
    normalizeVector->x = vector.x /lenght;
    normalizeVector->y = vector.y /lenght;
    normalizeVector->z = vector.z /lenght;
}

//dot product v1.x *v2.x + v1.y * v2.y + v1.z *v2.z

void crossProduct(struct vec3d *dest, struct vec3d v1, struct vec3d v2){

    dest->x = v1.y * v2.z - v1.z * v2.y;
    dest->y = v1.z * v2.x - v1.x * v2.z;
    dest->z = v1.x * v2.y - v1.y * v2.x;
}

void rotazioneZ(struct vec3d *input, struct vec3d *output, float angle){

    float matrice[4][4] = {0};



    matrice[0][0] = cosf(angle);
    matrice[1][1] = cosf(angle);
    matrice[2][2] = 1;
    matrice[0][1] = sinf(angle);
    matrice[1][0] = -sinf(angle);


    output->x = input->x * matrice[0][0] + input->y * matrice[1][0];
    output->y = input->x * matrice[0][1] + input->y * matrice[1][1];
    output->z = input->z;



}

void Matrix_MakeRotazioneX(float matrix[4][4], float fAngleRad){

    matrix[0][0] = 1.0f;
    matrix[1][1] =cosf(fAngleRad);
    matrix[1][2] = -sinf(fAngleRad);
    matrix[2][1] = sinf(fAngleRad);
    matrix[2][2] = cosf(fAngleRad);
    matrix[3][3] = 1.0f;
}


void Matrix_MakeRotationY(float matrix[4][4], float fAngleRad)
{

    matrix[0][0] = cosf(fAngleRad);
    matrix[0][2] = sinf(fAngleRad);
    matrix[2][0] = -sinf(fAngleRad);
    matrix[1][1] = 1.0f;
    matrix[2][2] = cosf(fAngleRad);
    matrix[3][3] = 1.0f;

}




void Matrix_MultiplyMatrix(float out[4][4], float m1[4][4], float m2[4][4]) {
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            out[r][c] = m1[r][0] * m2[0][c] + m1[r][1] * m2[1][c] + m1[r][2] * m2[2][c] + m1[r][3] * m2[3][c];
        }
    }
}



void proiezione(struct vec3d *input, struct vec3d *output){

    float matrice[4][4] = {0};
    float fNear = 1.0f;
    float fFar = 100.f;
    float fFOV = 90.0f;
    float fAspectRatio = 0.5f;
    float fFovRad = 1.0f/ tanf(fFOV* 0.5f/180.0f * 3.1415f);


    matrice[0][0] = fAspectRatio * fFovRad;
    matrice[1][1] = fFovRad;
    matrice[2][2] = fFar/ (fFar - fNear);
    matrice[3][2] = (-fFar * fNear) / (fFar - fNear);
    matrice[2][3] = 1.0f;
    matrice[3][3] = 0.0f;

    output->x = input->x * matrice[0][0] + input->y * matrice[1][0] + input->z * matrice[2][0] + matrice[3][0];
    output->y = input->x * matrice[0][1] + input->y * matrice[1][1] + input->z * matrice[2][1] + matrice[3][1];
    output->z = input->x * matrice[0][2] + input->y * matrice[1][2] + input->z * matrice[2][2] + matrice[3][2];

    float w = input->x * matrice[0][3] + input->y * matrice[1][3] + input->z * matrice[2][3] + matrice[3][3];


        output->x = output->x/w;
        output->y /= w;
        output->z /= w;




}





int main() {

    int width = 1600;
    int height = 800;

    int simulation_running = 1;

    FILE *file = fopen("C:\\Users\\Saimon\\CLionProjects\\engine3D\\FinalBaseMesh.obj", "r");
    if(file == NULL){
        perror("Errore opening file");
        return -1;
    }

    char *line = (char*) malloc(sizeof(char) * 256);


    char character;
    int size = 0;
    float v1,v2,v3;
    int index1,index2,index3,index4;

    int capacity,count;
    capacity = 8;
    count = 0;

    struct vec3d *objLoadedVertex;
    objLoadedVertex = (struct vec3d*) malloc(capacity * sizeof(struct vec3d));

    int capacityFace, countFace;
    capacityFace = 8;
    countFace = 0;
    struct triangle *objLoadedFace = (struct triangle*) malloc(capacityFace * sizeof(struct triangle));

    while((character = fgetc(file)) != EOF){

        if(size == 255){
            line[size] = '\0';

            size = 0;

        }
        if(character == '\n'){
            line[size] = '\0';

            if(line[0] == 'v'){

                sscanf(line, "v %f %f %f", &v1, &v2, &v3);




                if(count < capacity){
                    objLoadedVertex[count] = (struct vec3d) {v1, v2, v3};

                    count++;
                }else{
                    capacity = capacity * 2;
                    objLoadedVertex = realloc(objLoadedVertex, capacity * sizeof(struct vec3d));
                    objLoadedVertex[count] = (struct vec3d) {v1,v2,v3};
                    count++;
                }
            }

            if(line[0] == 'f'){
                sscanf(line, "f %d//%*d %d//%*d %d//%*d %d//%*d", &index1,  &index2, &index3 ,&index4);

                // f 8//8 7//7 9//9 10//10
                if(countFace  < capacityFace ){
                    objLoadedFace[countFace] = (struct triangle) {objLoadedVertex[index1-1],objLoadedVertex[index2-1],objLoadedVertex[index3-1]};
                    countFace++;
                    objLoadedFace[countFace] = (struct triangle) {objLoadedVertex[index1-1],objLoadedVertex[index3-1],objLoadedVertex[index4-1]};
                    countFace++;
                }else{
                    capacityFace = capacityFace* 2;
                    objLoadedFace = realloc(objLoadedFace, capacityFace * sizeof(struct triangle));
                    objLoadedFace[countFace] = (struct triangle) {objLoadedVertex[index1-1],objLoadedVertex[index2-1],objLoadedVertex[index3-1]};
                    countFace++;
                    objLoadedFace[countFace] = (struct triangle) {objLoadedVertex[index1-1],objLoadedVertex[index3-1],objLoadedVertex[index4-1]};
                    countFace++;
                }

            }



            size = 0;

        }else{
            if(size < 255){
                line[size] = character;
                size++;
            }

        }

    }
    free(line);
    printf("%d\n", capacity);
    printf("%d\n", count);

    struct mesh obj;
    obj.object = objLoadedFace;

    obj.numTriangle = countFace;

    SDL_Window *window = SDL_CreateWindow("3dEngine", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,
                                          width, height,SDL_WINDOW_VULKAN);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Event event;



    struct mesh Cubo;
    struct triangle *cubeTriangles = (struct triangle *)malloc(sizeof(struct triangle) * 12);
    struct triangle *pyramidTriangle = (struct triangle *)malloc(sizeof(struct triangle) * 6);

    if (cubeTriangles == NULL) {
        printf("Errore di allocazione memoria!\n");
    }



    Cubo.object = obj.object;
    Cubo.numTriangle = obj.numTriangle;


    struct mesh CuboProj;
    CuboProj.numTriangle = obj.numTriangle;
    struct triangle *cubeTriangleProj = (struct triangle *)malloc(obj.numTriangle * sizeof(struct triangle));
    CuboProj.object = cubeTriangleProj;


    struct vec3d camera = { 0,0,0};
    float matView[4][4];
    struct vec3d vLookDir = { 1,0,0 , 0};
    float cameraYaw = 0.0f;
    float cameraPitch = 0.0f * (3.14159f / 180.0f);
    struct vec3d up = {0,1,0};

    struct plane LEFT = {1,0,2,0};
    struct plane RIGHT = {-1,0,2,0};
    struct plane BOTTOM = {0,1,1,0};
    struct plane TOP = {0,-1,1,0};
    struct plane NEAR = {0,0,1,0.1f};
    //struct plane FAR = {0,0,1,100};

    printf("Prima della simulazione\n");
    while (simulation_running) {


        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Colore di sfondo nero
        SDL_RenderClear(renderer); // Pulisce lo schermo con il colore di sfondo
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);  // Bianco (R, G, B, A)






        normalize(&vLookDir, vLookDir);
        struct vec3d target = {0, 0, 1, 0};


        float matCameraRot[4][4] = {0};
        float matCameraRotX[4][4] = {0};
        float matCameraFinal[4][4] = {0};
        Matrix_MakeRotationY(matCameraRot, cameraYaw);
        Matrix_MakeRotazioneX(matCameraRotX, cameraPitch);
        Matrix_MultiplyMatrix(matCameraFinal, matCameraRotX, matCameraRot);


        vLookDir = Matrix_MultiplyDirection(matCameraFinal, target);  // w = 0


        normalize(&vLookDir, vLookDir);

        target.x = camera.x + vLookDir.x, target.y = camera.y + vLookDir.y, target.z = camera.z + vLookDir.z;

        viewMatrix(matView, &camera, &target, &up);


        while (SDL_PollEvent(&event)) {

            switch (event.type) {
                case SDL_QUIT:
                    simulation_running = 0;
                    break;

                case SDL_KEYDOWN:  // Quando un tasto viene premuto
                    switch (event.key.keysym.sym) {
                        case SDLK_UP:
                            printf("Freccia Su premuta!\n");
                            camera.z += 1;
                            break;
                        case SDLK_DOWN:
                            printf("Freccia Giù premuta!\n");
                            camera.z -= 1;
                            break;
                        case SDLK_LEFT:
                            //printf("Freccia Sinistra premuta!\n");
                            camera.x -= 1;
                            break;
                        case SDLK_RIGHT:
                            //printf("Freccia Destra premuta!\n");
                            camera.x += 1;
                            break;
                        case SDLK_SPACE:
                            camera.y += 0.2f;
                            break;
                        case SDLK_c:
                            camera.y -= 0.1f;
                            break;

                        case SDLK_f:
                            cameraPitch += 0.1f;
                            break;

                        case SDLK_g:
                            cameraPitch -= 0.1f;
                            break;


                        case SDLK_h:
                            cameraPitch = 0;
                            break;
                        case SDLK_w:

                            camera.x = camera.x + (vLookDir.x * 0.1f);
                            camera.y = camera.y + (vLookDir.y * 0.1f);
                            camera.z = camera.z + (vLookDir.z * 0.1f);
                            break;

                        case SDLK_s:

                            camera.x = camera.x - (vLookDir.x * 0.1f);
                            camera.y = camera.y - (vLookDir.y * 0.1f);
                            camera.z = camera.z - (vLookDir.z * 0.1f);
                            break;
                        case SDLK_a:
                            cameraYaw += 0.1f;
                            //printf("Freccia Destra premuta!\n");
                            break;

                        case SDLK_d:
                            cameraYaw -= 0.1f;
                            //printf("Freccia Destra premuta!\n");
                            break;


                        case SDLK_ESCAPE:  // Esci con ESC
                            simulation_running = 0;
                            break;
                    }
                    break;
            }

        }





        for (int i = 0; i < obj.numTriangle; ++i) {


            struct triangle tri = obj.object[i];


            tri.vertex[0] = MatrixMult(matView, tri.vertex[0]);
            tri.vertex[1] = MatrixMult(matView, tri.vertex[1]);
            tri.vertex[2] = MatrixMult(matView, tri.vertex[2]);


            // CALCOLO DELLA NORMALE DEL TRIANGOLO
            struct vec3d normal, line1, line2;

            // Calcolare i vettori dei lati del triangolo
            line1.x = tri.vertex[1].x - tri.vertex[0].x;
            line1.y = tri.vertex[1].y - tri.vertex[0].y;
            line1.z = tri.vertex[1].z - tri.vertex[0].z;

            line2.x = tri.vertex[2].x - tri.vertex[0].x;  // Corretto: tra il primo e il terzo vertice
            line2.y = tri.vertex[2].y - tri.vertex[0].y;
            line2.z = tri.vertex[2].z - tri.vertex[0].z;

            // Calcolare il prodotto vettoriale per ottenere la normale
            normal.x = line1.y * line2.z - line1.z * line2.y;
            normal.y = line1.z * line2.x - line1.x * line2.z;
            normal.z = line1.x * line2.y - line1.y * line2.x;

            // Normalizzare la normale
            float length = sqrtf(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
            normal.x /= length;
            normal.y /= length;
            normal.z /= length;

            // Calcolare il vettore che va dalla telecamera al triangolo (usando il primo vertice del triangolo)
            struct vec3d cameraRay = { tri.vertex[0].x,
                                       tri.vertex[0].y,
                                       tri.vertex[0].z};

            // Calcolare il prodotto scalare tra la normale e il vettore camera
            float dotProduct = normal.x * cameraRay.x + normal.y * cameraRay.y + normal.z * cameraRay.z;


            if(dotProduct < 0) {

                //CLIPPING


                struct triangle triClipped[256];
                int clippedTriangle;
                int sizeArray = 0;

                // Clip against NEAR plane first
                sizeArray = clippingPlane(tri, NEAR, triClipped, 0, 1);
                int tempsize = sizeArray;

                // Clip against TOP, BOTTOM, RIGHT, LEFT
                for (int j = 0; j < 4; ++j) {
                    int currentSize = tempsize;
                    tempsize = 0;
                    struct triangle newClipped[256];

                    for (int k = 0; k < currentSize; ++k) {
                        struct triangle triIn = triClipped[k];

                        // Choose the correct plane
                        struct plane p;
                        switch (j) {
                            case 0:
                                p = TOP;
                                break;
                            case 1:
                                p = BOTTOM;
                                break;
                            case 2:
                                p = RIGHT;
                                break;
                            case 3:
                                p = LEFT;
                                break;
                        }

                        // Clip current triangle against selected plane
                        clippedTriangle = clippingPlane(triIn, p, &newClipped[tempsize], 0, 1);

                        if (clippedTriangle == 1) {
                            tempsize++;
                        } else if (clippedTriangle == 2) {
                            tempsize += 2;
                        }
                        // If clippedTriangle == 0, do not copy anything (triangle discarded)
                    }

                    // Copy back for next iteration
                    for (int t = 0; t < tempsize; ++t) {
                        triClipped[t] = newClipped[t];
                    }
                }

                sizeArray = tempsize;
                // Proceed with sizeArray clipped triangles
                for (int f = 0; f < sizeArray; ++f) {

                    tri = triClipped[f];
                    drawFunc(tri, renderer);

                }

            }

        }

        SDL_RenderPresent(renderer);

    }

    free(objLoadedVertex);
    free(objLoadedFace);

    return 0;
}

