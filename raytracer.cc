//
//	This ray tracer is based on work by Peter Shirley, one of the authors of the text book Fundamentals of Computer Graphics.
//


#include <iostream>
#include <string>
#include <fstream>
#include "sphere.h"
#include "hitable_list.h"
#include "float.h"
#include "camera.h"
#include "material.h"
#include "texture.h"

using namespace std;
int numObject=0;
int nx;
int ny;
int ns;
vec3 up;
vec3 camera2;
vec3 center;
hitable *list[1000];

vec3 color(const ray& r, hitable *world, int depth) {
    hit_record rec;

#ifdef WIN32
	if (world->hit(r, 0.001, FLT_MAX, rec)) {
#else
	if (world->hit(r, 0.001, MAXFLOAT, rec)) {
#endif
        ray scattered;
        vec3 attenuation;
        if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
             return attenuation*color(scattered, world, depth+1);
        }
        else {
            return vec3(0,0,0);
        }
    }
    else {
        vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5*(unit_direction.y() + 1.0);
        return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
    }
}
    
    void readFile(string s){
        
        string word;
        ifstream file;
        file.open (s);
        vec3 cen;
        vec3 lamb;
        vec3 meta;
        vec3 tex1;
        vec3 tex2;
        float radius;
        float ior;
        if (!file.is_open()){
            cout<<"FILE NOT FOUND"<<endl;
            return;
        }
        
        while (file >> word)
        {

            if(word.compare("nx")==0){
                
                file >> word;
                nx=::atof(word.c_str());
                
            }
            else if(word.compare("ny")==0){
                
                file >> word;
                ny=::atof(word.c_str());
                
            }
            else if(word.compare("ns")==0){
                file >> word;
                ns=::atof(word.c_str());
                
            }
            else if(word.compare("camera")==0){
                file >> word;
                camera2[0]=::atof(word.c_str());
                file >> word;
                camera2[1]=::atof(word.c_str());
                file >> word;
                camera2[2]=::atof(word.c_str());
                
            }
            else if(word.compare("viewup")==0){
                file >> word;
                up[0]=::atof(word.c_str());
                file >> word;
                up[1]=::atof(word.c_str());
                file >> word;
                up[2]=::atof(word.c_str());
                
            }
            else if(word.compare("center")==0){
                file >> word;
                center[0]=::atof(word.c_str());
                file >> word;
                center[1]=::atof(word.c_str());
                file >> word;
                center[2]=::atof(word.c_str());
                
            }
            else if(word.compare("sphere")==0){
                file >> word;
                if (word.compare("center")==0) {
                    
                    file >> word;
                    cen[0]=::atof(word.c_str());
                    file >> word;
                    cen[1]=::atof(word.c_str());
                    file >> word;
                    cen[2]=::atof(word.c_str());
                }
                else{
                
                    cout<<"error"<<endl;
                }
            
            }
            else if(word.compare("radius")==0){
                file >> word;
                radius=::atof(word.c_str());
               
            }
            else if(word.compare("material")==0){
                file >> word;
                if(word.compare("lambertian")==0){
                    file >> word;
                    if(word.compare("albedo")==0){
                        file >> word;
                        lamb[0]=::atof(word.c_str());
                        file >> word;
                        lamb[1]=::atof(word.c_str());
                        file >> word;
                        lamb[2]=::atof(word.c_str());
                        list[numObject]= new sphere(cen, radius, new lambertian(new constant_texture(lamb)));
                        numObject++;
//                        cout<<cen[0]<<endl;
//                        cout<<cen[1]<<endl;
//                        cout<<cen[2]<<endl;
//                        cout<<radius<<endl;
//                        cout<<lamb[0]<<endl;
//                        cout<<lamb[1]<<endl;
//                        cout<<lamb[2]<<endl;
//                        cout<<"========="<<endl;
                        
                    }
                    else if(word.compare("checker_texture")==0){
                        file >> word;
                        tex1[0]=::atof(word.c_str());
                        file >> word;
                        tex1[1]=::atof(word.c_str());
                        file >> word;
                        tex1[2]=::atof(word.c_str());
                        
                        file >> word;
                        tex2[0]=::atof(word.c_str());
                        file >> word;
                        tex2[1]=::atof(word.c_str());
                        file >> word;
                        tex2[2]=::atof(word.c_str());
                        texture *checker = new checker_texture(new constant_texture(tex1),new constant_texture(tex2));
                        list[numObject]= new sphere(cen, radius, new lambertian(checker));
                        numObject++;
                    }
                    
                }
                else if(word.compare("metal")==0){
                    file >> word;
                    if(word.compare("albedo")==0){
                        file >> word;
                        meta[0]=::atof(word.c_str());
                        file >> word;
                        meta[1]=::atof(word.c_str());
                        file >> word;
                        meta[2]=::atof(word.c_str());
                        file >> word;
                        int fuzz=::atof(word.c_str());
                        list[numObject] = new sphere(cen, radius, new metal(meta,fuzz));
                        numObject++;
                    }
                }
                else if(word.compare("dielectric")==0){
                    file >> word;
                    if(word.compare("ior")==0){
                        file >> word;
                        ior=::atof(word.c_str());
                        list[numObject] = new sphere(cen, radius, new dielectric(ior));
                        numObject++;
                    }
                }
            
            }
        }
            
}
        
        

int main(int argc, char **argv) {

    float R = cos(M_PI/4);
    if (argc!=2) {
        std::cout << "ERROR: please provide a scene file"<<endl;
        return 1;
    }
    string s=argv[1];
    readFile(s);
    std::cout << "P3\n" << nx << " " << ny << "\n255\n";	// header for PPM file
    hitable *world = new hitable_list(list,numObject);
    
    float dist_to_focus = 10.0;
    float aperture = 0.1;

    camera cam(camera2, center, up, 20, float(nx)/float(ny), aperture, dist_to_focus);
    
    for (int j = ny-1; j >= 0; j--) {		// for each line, top to bottom
		if (j % 10 == 0) fprintf(stderr, "On line: %d\n", j);		// informational print
        for (int i = 0; i < nx; i++) {		// for each pixel on the line
            vec3 col(0, 0, 0);
            for (int s=0; s < ns; s++) {	// samples within a pixel
                float u = float(i + drand48()) / float(nx);
                float v = float(j + drand48()) / float(ny);
                ray r = cam.get_ray(u, v);
                vec3 p = r.point_at_parameter(2.0);
                col += color(r, world,0);
            }
            col /= float(ns);	// adjust for number of samples
            col = vec3( sqrt(col[0]), sqrt(col[1]), sqrt(col[2]) );
            int ir = int(255.99*col[0]); 
            int ig = int(255.99*col[1]); 
            int ib = int(255.99*col[2]); 
            std::cout << ir << " " << ig << " " << ib << "\n";
        }
    }
}



