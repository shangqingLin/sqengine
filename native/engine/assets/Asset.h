#ifndef _ASSET_H_
#define _ASSET_H_
#include <string>

class Asset{
  protected:
    int id;
    std::string keyUrl;
  public:
      Asset(int id);
      virtual ~Asset();
      int getId(){return id;};
      virtual void setKeyUrl(char* url);
      inline const std::string& getKeyUrl(){ return keyUrl; };
};
#endif