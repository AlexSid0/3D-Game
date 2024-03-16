#include <iostream>
#include <SDL2/SDL.h>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <variant>
#include <iterator>
#include <bits/stdc++.h> 

struct Object
{
    std::vector<std::vector<double>> vert;
    std::vector<std::vector<int>> face;
    std::vector<std::vector<int>> normals;
    std::vector<std::vector<double>> faceNormals;
};

const int focal = 500;
const int multiply = 100;
const int speed = 100;

int WIDTH = 1920;
int HEIGHT = 1080;
std::vector<Object> Objects;

//realtime data
std::vector<bool> keyboard = {false,false,false,false}; //W, A, S, D
std::vector<double> cameraPos = {0,0,0};
std::vector<double> objectCameraOffset = {0,0,0};

void printVector(std::vector<double> vector){
    using namespace std;
    copy(vector.begin(), vector.end(),
    ostream_iterator<double>(cout, " "));
}

void printIntVector(std::vector<int> vector){
    using namespace std;
    copy(vector.begin(), vector.end(),
    ostream_iterator<int>(cout, " "));
}

void move(double deltaTime){
    cameraPos[2] += keyboard[0] * speed * deltaTime;
    cameraPos[0] -= keyboard[1] * speed * deltaTime;
    cameraPos[2] -= keyboard[2] * speed * deltaTime;
    cameraPos[0] += keyboard[3] * speed * deltaTime;

    int vecSize = objectCameraOffset.size();
    for (int i = 0; i < vecSize; ++i) {
        objectCameraOffset[i] = cameraPos[i] *-1;
    }
}

std::vector<double> calculateVector(std::string data){
    using namespace std;
    
    vector<double> vector;

    int length = data.length();//convert string to
    char str[length + 1];
    strcpy(str, data.c_str());
    
    char *ptr;
    ptr = strtok(str, " ");
    
    while (ptr != NULL)  
    {    
        vector.push_back(stod(ptr));
        ptr = strtok (NULL, " ");  
    }
    
    return vector;
}

std::vector<int> calculateVectorInt(std::string data){
    using namespace std;
    
    vector<int> vector;

    int length = data.length();//convert string to
    char str[length + 1];
    strcpy(str, data.c_str());
    
    char *ptr;
    ptr = strtok(str, " ");
    
    while (ptr != NULL)  
    {    
        vector.push_back(stoi(ptr));
        ptr = strtok (NULL, " ");  
    }
    
    return vector;
}

std::vector<std::string> calculateVectorSections(std::string data){
    using namespace std;
    
    vector<string> vector;

    int length = data.length();//convert string to
    char str[length + 1];
    strcpy(str, data.c_str());
    
    char *ptr;
    ptr = strtok(str, " ");
    
    while (ptr != NULL)  
    {    
        vector.push_back(ptr);
        ptr = strtok (NULL, " ");  
    }
    
    return vector;
}

std::vector<int> calculateVectorPointers(std::string data){
    using namespace std;
    
    vector<int> vector;

    int length = data.length();//convert string to
    char str[length + 1];
    strcpy(str, data.c_str());
    
    char *ptr;
    ptr = strtok(str, "/");
    while (ptr != NULL)  
    {    
        vector.push_back(stoi(ptr));
        ptr = strtok (NULL, "/");  
    }
    return vector;
}

void loadModel(std::string b){
    using namespace std;

    b = b+".obj";
    ifstream f(b);
    if(f.is_open()){
        string fileLine; //gets all lines from file
        Object object;
        vector<vector<double>> vert;
        vector<vector<int>> face;
        vector<vector<int>> normalPointers;
        vector<vector<double>> faceNormals;
       
        while(getline(f, fileLine)){

            int length = fileLine.length();
            
            if(fileLine.find("vn ") != string::npos){
                faceNormals.push_back(calculateVector(fileLine.substr(3,length)));
            }
        
            if(fileLine.find("v ") != string::npos){
                vert.push_back(calculateVector(fileLine.substr(2,length))); 
            }
        
            if(fileLine.find("f ") != string::npos){
                vector<string> sections;
                sections = calculateVectorSections(fileLine.substr(2,length));

                vector<int> section1 = calculateVectorPointers(sections[0]);
                vector<int> section2 = calculateVectorPointers(sections[1]);
                vector<int> section3 = calculateVectorPointers(sections[2]);

                face.push_back({section1[0],section2[0],section3[0]});
                normalPointers.push_back({section1[2],section2[2],section3[2]});
            }
        
        }
        f.close();
        object.vert = vert;
        object.face = face;
        object.normals = normalPointers;
        object.faceNormals = faceNormals;
        Objects.push_back(object);
    }
    else
    {
        cout << "failed to open file :( \n";
        cout << "do you want to try again [Y/N]?\n";
        string i;
        cin >> i;
        if(i == "Y"){
            loadModel(b);
        }
    }
}

void line(std::vector<double> v,std::vector<double> v1,SDL_Renderer *renderer){
    //convert to middle screen pixels
    v[0] += WIDTH/2;// dont ask, i could not find a better way to do this
    v[1] += HEIGHT/2; 
    v1[0] += WIDTH/2;
    v1[1] += HEIGHT/2; 

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawLine(renderer, v[0],v[1],v1[0],v1[1]);
}

void triangle(float v[],float v1[],float v2[],uint8_t c[],uint8_t c1[],uint8_t c2[],float multiplier,SDL_Renderer *renderer){
    //convert to middle screen pixels
    v[0] += WIDTH/2;// dont ask, i could not find a better way to do this
    v[1] += HEIGHT/2; 
    v1[0] += WIDTH/2;
    v1[1] += HEIGHT/2; 
    v2[0] += WIDTH/2;
    v2[1] += HEIGHT/2;
    c[0] *= multiplier;
    c[1] *= multiplier; 
    c[2] *= multiplier;
    c1[0] *= multiplier; 
    c1[1] *= multiplier;
    c1[2] *= multiplier;
    c2[0] *= multiplier; 
    c2[1] *= multiplier;
    c2[2] *= multiplier;

    SDL_Vertex vertex_1 = {{v[0],v[1]}, {c[0], c[1], c[2], 255}, {1, 1}};
    SDL_Vertex vertex_2 = {{v1[0],v1[1]}, {c1[0], c1[1], c1[2], 255}, {1, 1}};
    SDL_Vertex vertex_3 = {{v2[0],v2[1]}, {c2[0], c2[1], c2[2], 255}, {1, 1}};//set colors
    SDL_Vertex vertices[] = {vertex_1,vertex_2,vertex_3};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderGeometry(renderer, nullptr, vertices, 3, NULL, 0);//draw
}

void render(std::vector<Object> scene,SDL_Renderer *renderer){
    for(int o=0; o < scene.size(); o++){//every object
        std::vector<std::vector<double>> usedVerts;
        usedVerts.reserve(scene[o].vert.size());

        for(int f=0; f < scene[o].face.size(); f++){
            std::vector<double> v1,v2,v3,pos1,pos2, pos3;
            
            int v1Index = scene[o].face[f][0]-1;
            int v2Index = scene[o].face[f][1]-1;
            int v3Index = scene[o].face[f][2]-1;
            std::cout << "1\n";
            if(usedVerts[v1Index].empty()){
                v1 = scene[o].vert[scene[o].face[f][0]-1];
                for(int i = 0; i < 3; i++){
                    v1[i] *= multiply;
                    v1[i] += 10;
                    v1[i] += objectCameraOffset[i];
                }
            }
            std::cout << "2\n";
            if(usedVerts[v2Index].empty()){
                v2 = scene[o].vert[scene[o].face[f][1]-1];
                for(int i = 0; i < 3; i++){
                    v2[i] *= multiply;
                    v2[i] += 10;
                    v2[i] += objectCameraOffset[i];
                }
            }
            std::cout << "3\n";
            if(usedVerts[v3Index].empty()){
                v3 = scene[o].vert[scene[o].face[f][2]-1];
                for(int i = 0; i < 3; i++){
                    v3[i] *= multiply;
                    v3[i] += 10;
                    v3[i] += objectCameraOffset[i];
                }
            }
            std::cout << "4\n";
            ///////normal calculation
            //rotation vertex
            //roation normal
            //clipping code
            std::cout << v2Index;
            printVector(usedVerts[v2Index]);
            if(usedVerts[v1Index].empty()){
                pos1 = {(focal * v1[0]) / (focal + v1[2]),(focal * v1[1]) / (focal + v1[2])};
                usedVerts.at(v1Index) = pos1;
            }
            else{
                pos1 = usedVerts.at(v1Index);
            }
            std::cout << "5\n";
            if(usedVerts[v2Index].empty()){
                pos2 = {(focal * v2[0]) / (focal + v2[2]),(focal * v2[1]) / (focal + v2[2])};
                usedVerts.at(v2Index) = pos2;
            }
            else{
                pos2 = usedVerts.at(v2Index);
            }
            std::cout << "6\n";
            if(usedVerts[v3Index].empty()){
                pos3 = {(focal * v3[0]) / (focal + v3[2]),(focal * v3[1]) / (focal + v3[2])};
                usedVerts.at(v3Index) = pos3;
            }
            else{
                pos3 = usedVerts.at(v3Index);
            }
            std::cout << "7\n";
            line(pos1,pos2,renderer);
            line(pos2,pos3,renderer);
            line(pos3,pos1,renderer);
        }
    }
}

void screenSize(){
    std::string answer;
    std::cout << "Do you want to use default settings [Y/N]? \n";
    std::cin >> answer;

    if(answer == "N"){
        std::cout << "WIDTH THEN HEIGHT OF YOUR MONITOR";
        std::cout << "\n";
        std::cin >> WIDTH;
        std::cin >> HEIGHT;
    }
    else if (answer != "Y")
    {
        std::cout << "\n";
        screenSize();
    }
}

int main( int argc,char*argv[]){
    
    screenSize();
    
    loadModel("cube");
    //loadModel("ERRORCUBE");
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Event windowEvent;
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    SDL_CreateWindowAndRenderer(WIDTH,HEIGHT,0,&window,&renderer);

    Uint64 FPSCounter = SDL_GetPerformanceCounter();//delta related
    Uint64 Frequency = SDL_GetPerformanceFrequency();
    double deltaTime = 1;

    //main code
    while(true){
        SDL_SetRenderDrawColor(renderer,0,0,0,255);//reset
        SDL_RenderClear(renderer);
        
        if(SDL_PollEvent(&windowEvent)){ // handle input
            if(SDL_QUIT == windowEvent.type)
            {
                break;
            }

            if(windowEvent.type == SDL_KEYDOWN)
            {
                switch(windowEvent.key.keysym.sym)
                {
                    case SDLK_w:
                        keyboard[0] = true;
                        break;
                    case SDLK_s:
                        keyboard[2] = true;
                        break;
                    case SDLK_a:
                        keyboard[1] = true;
                        break;
                    case SDLK_d:
                        keyboard[3] = true;
                        break;
                }
            }
            if(windowEvent.type == SDL_KEYUP)
            {
                switch(windowEvent.key.keysym.sym)
                {
                    case SDLK_w:
                        keyboard[0] = false;
                        break;
                    case SDLK_s:
                        keyboard[2] = false;
                        break;
                    case SDLK_a:
                        keyboard[1] = false;
                        break;
                    case SDLK_d:
                        keyboard[3] = false;
                        break;
                }
            }
        }
        move(deltaTime);

        SDL_SetRenderDrawColor(renderer,255,255,255,255);//set color of line
        render(Objects,renderer);//render
        SDL_RenderPresent(renderer);//show rendered result
        

        //delta time
        Uint64 EndTime = SDL_GetPerformanceCounter();
        Uint64 elapsed = (EndTime-FPSCounter);
        FPSCounter = EndTime;
        deltaTime = (double)elapsed/(double)Frequency;
    }
    std::cout << "BUTTON: CLOSE";
    return EXIT_SUCCESS;
}