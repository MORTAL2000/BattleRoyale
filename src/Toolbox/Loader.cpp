#include "Loader.hpp"
#include "Model/Mesh.hpp"
#include "Model/Texture.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <vector>

Texture Loader::loadTexture(const std::string fileName) {
   Texture texture;
   std::map<std::string, GLint>::iterator it = textures.find(fileName);
   if (it != textures.end()) {
      texture.textureId = it->second;
   }
   else {
      int sizeX, sizeY, comp;
      stbi_set_flip_vertically_on_load(true);
      unsigned char *data = stbi_load(fileName.c_str(), &sizeX, &sizeY, &comp, STBI_rgb_alpha);
      if(data) {
         std::cout << "Loaded texture (" << sizeX << ", " << sizeY << "): " << fileName << std::endl;
         texture.init(sizeX, sizeY, comp, data);
         if (texture.textureId) {
            textures.insert(std::map<std::string, GLint>::value_type(fileName, texture.textureId));
         }
         stbi_image_free(data);
      }
      else {
         std::cerr << "Could not find " << fileName << std::endl;
      }
   }
   return texture;
}

// Load geometry from .obj
std::vector<Mesh*> Loader::loadObjMesh(const std::string fileName) {
   std::map<std::string, std::vector<Mesh*>>::iterator it = meshes.find(fileName);
   if (it != meshes.end()) {
      return it->second;
   }

   std::vector<tinyobj::shape_t> shapes;
   std::vector<tinyobj::material_t> objMaterials;
   std::string errString;
   bool rc = tinyobj::LoadObj(shapes, objMaterials, errString, fileName.c_str());
   if (!rc) {
      std::cerr << errString << std::endl;
      exit(1);
   }
 
   /* Organize all submeshes in the provided .obj file */
   std::vector<Mesh*> meshList;
   int vertCount = 0;
   for (int i = 0; i < shapes.size(); i++) {
      Mesh *mesh = new Mesh;
      mesh->name = fileName;
      mesh->vertBuf = shapes[i].mesh.positions;
      mesh->norBuf  = shapes[i].mesh.normals;
      mesh->texBuf  = shapes[i].mesh.texcoords;
      mesh->eleBuf  = shapes[i].mesh.indices; 
      meshList.push_back(mesh);
      vertCount += mesh->vertBuf.size()/3;
   }

   /* Resize the submeshes */
   resize(meshList);

   /* Copy mesh data to the gpu */
   for (Mesh *mesh : meshList) {
      mesh->init();
   }
   
   meshes.insert(std::map<std::string, std::vector<Mesh*>>::value_type(fileName, meshList));

   std::cout << "Loaded mesh (" << vertCount << " vertices): " << fileName << std::endl;

   return meshList;
}

// Provided function to resize a mesh so all vertex positions are [0, 1.f]
void Loader::resize(std::vector<Mesh *> meshes) {
   float minX, minY, minZ;
   float maxX, maxY, maxZ;
   float scaleX, scaleY, scaleZ;
   float shiftX, shiftY, shiftZ;
   float epsilon = 0.001;

   minX = minY = minZ = 1.1754E+38F;
   maxX = maxY = maxZ = -1.1754E+38F;

   for (Mesh *mesh : meshes) {
      //Go through all vertices to determine min and max of each dimension
      for (size_t v = 0; v < mesh->vertBuf.size() / 3; v++) {
         if(mesh->vertBuf[3*v+0] < minX) minX = mesh->vertBuf[3*v+0];
         if(mesh->vertBuf[3*v+0] > maxX) maxX = mesh->vertBuf[3*v+0];

         if(mesh->vertBuf[3*v+1] < minY) minY = mesh->vertBuf[3*v+1];
         if(mesh->vertBuf[3*v+1] > maxY) maxY = mesh->vertBuf[3*v+1];

         if(mesh->vertBuf[3*v+2] < minZ) minZ = mesh->vertBuf[3*v+2];
         if(mesh->vertBuf[3*v+2] > maxZ) maxZ = mesh->vertBuf[3*v+2];
      }
   }
   //From min and max compute necessary scale and shift for each dimension
   float maxExtent, xExtent, yExtent, zExtent;
   xExtent = maxX-minX;
   yExtent = maxY-minY;
   zExtent = maxZ-minZ;
   if (xExtent >= yExtent && xExtent >= zExtent) {
   maxExtent = xExtent;
   }
   if (yExtent >= xExtent && yExtent >= zExtent) {
      maxExtent = yExtent;
   }
   if (zExtent >= xExtent && zExtent >= yExtent) {
      maxExtent = zExtent;
   }
   scaleX = 2.0 /maxExtent;
   shiftX = minX + (xExtent/ 2.0);
   scaleY = 2.0 / maxExtent;
   shiftY = minY + (yExtent / 2.0);
   scaleZ = 2.0/ maxExtent;
   shiftZ = minZ + (zExtent)/2.0;

   //Go through all verticies shift and scale them
   for (Mesh *mesh : meshes) {
   	for (size_t v = 0; v < mesh->vertBuf.size() / 3; v++) {
   		mesh->vertBuf[3*v+0] = (mesh->vertBuf[3*v+0] - shiftX) * scaleX;
   		assert(mesh->vertBuf[3*v+0] >= -1.0 - epsilon);
   		assert(mesh->vertBuf[3*v+0] <= 1.0 + epsilon);
   		mesh->vertBuf[3*v+1] = (mesh->vertBuf[3*v+1] - shiftY) * scaleY;
   		assert(mesh->vertBuf[3*v+1] >= -1.0 - epsilon);
   		assert(mesh->vertBuf[3*v+1] <= 1.0 + epsilon);
   		mesh->vertBuf[3*v+2] = (mesh->vertBuf[3*v+2] - shiftZ) * scaleZ;
   		assert(mesh->vertBuf[3*v+2] >= -1.0 - epsilon);
         assert(mesh->vertBuf[3*v+2] <= 1.0 + epsilon);
   	}
   }
}