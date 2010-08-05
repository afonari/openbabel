#include "obtest.h"

#include <openbabel/mol.h>
#include <openbabel/obconversion.h>
#include <openbabel/isomorphism.h>
#include <openbabel/query.h>

using namespace std;
using namespace OpenBabel;

std::string GetFilename(const std::string &filename)
{
  string path = TESTDATADIR + filename;
  return path;
}

void testIsomorphism1()
{
  OBMol mol;
  OBConversion conv;
  conv.SetInFormat("smi");
  conv.ReadString(&mol, "CC1CCC(C)CC1");

  OBQuery *query = CompileMoleculeQuery(&mol);
  OBIsomorphismMapper *mapper = OBIsomorphismMapper::GetInstance(query);
  OBIsomorphismMapper::Mappings maps = mapper->MapAll(&mol);

  OB_ASSERT( maps.size() == 4 );

  delete query;
  delete mapper;

  query = CompileSmilesQuery("C1(C)CCC(C)CC1");
  mapper = OBIsomorphismMapper::GetInstance(query);
  
  OB_ASSERT( mapper->MapFirst(&mol).size() == 8 );
  OB_ASSERT( mapper->MapUnique(&mol).size() == 1 );
  OB_ASSERT( mapper->MapAll(&mol).size() == 4 );

  delete query;
  delete mapper;
}

void testIsomorphism2()
{
  OBMol mol;
  OBConversion conv;
  conv.SetInFormat("smi");
  conv.ReadString(&mol, "Cc1ccc(C)cc1");

  OBQuery *query = CompileSmilesQuery("C1=CC=CC=C1");
  OBIsomorphismMapper *mapper = OBIsomorphismMapper::GetInstance(query);
  OBIsomorphismMapper::Mappings maps = mapper->MapUnique(&mol);

  cout << maps.size() << endl;

  OB_ASSERT( maps.size() == 1 );

  delete query;
  delete mapper;
}

void testIsomorphismMask()
{
  // read file: 3 6-rings
  //
  //     /\ /\ /\
  //    |  |  |  |
  //     \/ \/ \/
  //
  OBMol mol;
  OBConversion conv;
  conv.SetInFormat("cml");
  std::ifstream ifs(GetFilename("isomorphism1.cml").c_str());
  OB_REQUIRE( ifs );
  conv.Read(&mol, &ifs);

  OBQuery *query = CompileSmilesQuery("C1CCCCC1");
  OBIsomorphismMapper *mapper = OBIsomorphismMapper::GetInstance(query);
  
  // no mask
  OBIsomorphismMapper::Mappings maps = mapper->MapUnique(&mol);
  cout << maps.size() << endl;
  OB_ASSERT( maps.size() == 3 );

  // mask first ring
  OBBitVec mask;
  for (int i = 0; i < 6; ++i)
    mask.SetBitOn(i+1);
  maps = mapper->MapUnique(&mol, mask);
  cout << maps.size() << endl;
  OB_ASSERT( maps.size() == 1 );

  // mask second ring also
  for (int i = 6; i < 10; ++i)
    mask.SetBitOn(i+1);
  maps = mapper->MapUnique(&mol, mask);
  cout << maps.size() << endl;
  OB_ASSERT( maps.size() == 2 );


  delete query;
  delete mapper;
}


int main() 
{
  // Define location of file formats for testing
  #ifdef FORMATDIR
    char env[BUFF_SIZE];
    snprintf(env, BUFF_SIZE, "BABEL_LIBDIR=%s", FORMATDIR);
    putenv(env);
  #endif  

  testIsomorphism1();
  testIsomorphism2();
  testIsomorphismMask();

  return 0;
}

                