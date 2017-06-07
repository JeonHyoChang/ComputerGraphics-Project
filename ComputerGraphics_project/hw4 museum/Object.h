#pragma once
#include <string>
#include <vector>
#include <map>

#include <SOIL.h>
#include <glm/glm.hpp>

struct Group
{
public:
  Group(const std::string& name);
    
public:
  std::string               m_name;

  std::vector<glm::vec3>    m_vertices;
  std::vector<glm::vec3>    m_normals;
  std::vector<glm::vec2>    m_texcoords;
  std::string               m_mtl_name;
};

struct Material
{
public:  
  Material();
  Material(const std::string& name, 
    glm::vec4& ambient, glm::vec4& diffuse, glm::vec4& specular, float& shininess);
  
public:
  std::string     m_name;

  glm::vec4       m_ambient;
  glm::vec4       m_diffuse;
  glm::vec4       m_specular;
  float           m_shininess;
};

class Object
{
public:
  Object() {}

  void draw(int loc_a_vertex, int loc_a_normal) ;
  void print();
	
	bool load_simple_obj(const std::string& filename, std::string imgname);
  bool load_simple_mtl(const std::string& filename);

private:  
  GLuint    programE;
  GLint     loc_material_ambient;
GLint     loc_material_diffuse;
GLint     loc_material_specular;
GLint     loc_material_shininess;
GLint       loc_u_texid;
GLint       loc_a_texcoord;
GLuint      textureid;
  std::string PATH;
  std::vector<Group>              m_groups;
  std::map<std::string, Material> m_materials;  
};