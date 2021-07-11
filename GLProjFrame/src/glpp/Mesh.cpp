#include "glpp/Mesh.h"

void Scene::init(const string& filename){
    // Release the previously loaded mesh (if it exists)
    if(loaded) {
        this -> clear();
        loaded=false;
    }

    Assimp::Importer importer;

    const aiScene* pAiScene = importer.ReadFile(
        filename.c_str(), 
        aiProcess_Triangulate | 
        aiProcess_GenSmoothNormals | 
        aiProcess_FlipUVs | 
        aiProcess_JoinIdenticalVertices
    );

    if (!pAiScene) {
        char buf[100];
        sprintf_s(buf,100u,"Error parsing '%s': '%s'\n", filename.c_str(), importer.GetErrorString());
        Logging::error(buf);
        return;
    }
    this -> init(pAiScene);
}

void Scene::init(const aiScene* pAiScene, const string& filename){
    if(loaded) {
        this -> clear();
        loaded=false;
    }

    //init meshes
    meshes.resize(pAiScene->mNumMeshes);

    for(int i=0;i<pAiScene->mNumMeshes;i++){
        meshes[i].setFather(this);
        meshes[i].init(pAiScene->mMeshes[i]);
    }

     //loading materials
    materials.resize(pAiScene->mNumMaterials);
    std::string::size_type slashIndex = filename.find_last_of("/");
    std::string dir;
    if (slashIndex == std::string::npos) {
        dir = ".";
    }
    else if (slashIndex == 0) {
        dir = "/";
    }
    else {
        dir = filename.substr(0, slashIndex);
    }//get dir

    for(int i=0;i<pAiScene->mNumMaterials;i++){
        const aiMaterial* pMat = pAiScene->mMaterials[i];
        if(pMat->GetTextureCount(aiTextureType_DIFFUSE)>0){//这是一个非常简化的版本，只考虑材料中的第一个diffuse_texture，日后可能改进
            aiString path;
            if(pMat->GetTexture(
                aiTextureType_DIFFUSE,0,&path,
                NULL,NULL,NULL,NULL,NULL
            )!=AI_SUCCESS){
                Logging::error("assimp failed to load diffuse texture from material!");
            }
            string fullPath = dir + "/" + path.data;
            materials[i]=new Texture(fullPath);
        }
    }
}

Scene::~Scene(){
    for(auto pMat:materials){
        delete pMat;
    }
}

void Mesh::init(const aiMesh* pAiMesh){
    numVertices = pAiMesh->mNumVertices;
    numIndices = pAiMesh->mNumFaces*3;

    materialIndex = pAiMesh->mMaterialIndex;

    vertices = new float(pAiMesh->mNumVertices*8);    
    indices = new int(pAiMesh->mNumFaces*3);

    for(int vi=0;vi<pAiMesh->mNumVertices;vi++){
        vertices[8*vi  ]=pAiMesh->mVertices[vi].x;
        vertices[8*vi+1]=pAiMesh->mVertices[vi].y;
        vertices[8*vi+2]=pAiMesh->mVertices[vi].z;
        vertices[8*vi+3]=pAiMesh->mNormals[vi].x;
        vertices[8*vi+4]=pAiMesh->mNormals[vi].y;
        vertices[8*vi+5]=pAiMesh->mNormals[vi].z;
        if(pAiMesh->HasTextureCoords(0)){
            vertices[8*vi+6]=pAiMesh->mTextureCoords[0][vi].x;
            vertices[8*vi+7]=pAiMesh->mTextureCoords[0][vi].y;
        }else{
            vertices[8*vi+6]=0.0f;
            vertices[8*vi+7]=0.0f;
        }
    }

    for(int fi=0;fi<pAiMesh->mNumFaces;fi++){
        const aiFace& face = pAiMesh->mFaces[fi];
        assert(face.mNumIndices == 3);
        indices[fi*3  ]=face.mIndices[0];
        indices[fi*3+1]=face.mIndices[1];
        indices[fi*3+2]=face.mIndices[2];
    }//2021-7-10: lcj: 只写完了从aiScene把大段数据弄出来的代码
    //material index都没有加入mesh
    //一个小问题，meterial index 使用AOS还是SOA存储呢？

   

}

Mesh::~Mesh(){
    delete[] vertices;
    delete[] indices;
}