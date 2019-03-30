/*****************************************************************************/
/* DLM модуль для RSL                                                        */          
/* на основе шаблона классов ptree библиотеки  boost.                        */
/* версия 0.0.0 (27.01.2017)                                                 */
/* версия 0.0.1 (05.05.2017)                                                 */
/* tema@mail.ru                                                              */
/*****************************************************************************/

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <string>
#include <set>
#include <exception>
#include <iostream>

#include <Windows.h>
#include "mssup.h"
#include "rsl/dlmintf.h"

using namespace boost::property_tree;

class  TPropTree
{
public:
    std::string UTF8_to_CP866(std::string const & utf8) {
        if (disableToOEM.value.boolval) return std::string(utf8);
        if(!utf8.empty()) {
            int wchlen = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), utf8.size(), NULL, 0);
            if(wchlen > 0 && wchlen != 0xFFFD)
            {
                std::vector<wchar_t> wbuf(wchlen);
                MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), utf8.size(), &wbuf[0], wchlen);
                std::vector<char> buf(wchlen);
                WideCharToMultiByte(866, 0, &wbuf[0], wchlen, &buf[0], wchlen, 0, 0);
     
                return std::string(&buf[0], wchlen);
            }
        }
        return std::string();
    }


    TPropTree (TGenObject *pThis = NULL) {
        }

   ~TPropTree () {
        }

    void setPTree(ptree p) {
        pt=p;
        };


    RSL_CLASS(TPropTree)

    RSL_METHOD_DECL(LoadXml) { 
        strLastError="";
        VALUE *vFile;
        int iRet=0;
        if (GetParm (1,&vFile) && vFile->v_type == V_STRING) {
            std::string strFileName(vFile->value.string);
            OemToCharBuff((char *)strFileName.data(),(char *)strFileName.data(),strFileName.size());

            try {
                read_xml(strFileName, pt, 0, std::locale(strXmlLocale)); 
                iRet=1;
                ReturnVal (V_BOOL,&iRet);
                }
            catch (std::exception &e) {
                strLastError=e.what();

                ReturnVal (V_UNDEF,&iRet);
                }
            catch (...) {
                ReturnVal (V_BOOL,&iRet);
                }
            }
        else {
            RslError("param 1 must be string");
            ReturnVal (V_BOOL,&iRet);
            }

        it=pt.not_found();
        iti=pt.end(); 

        return 0;
        }

    RSL_METHOD_DECL(LoadJson) { 
        strLastError="";
        VALUE *vFile;
        int iRet=0;
        if (GetParm (1,&vFile) && vFile->v_type == V_STRING) {
            std::string strFileName(vFile->value.string);
            OemToCharBuff((char *)strFileName.data(),(char *)strFileName.data(),strFileName.size());
            try {
                read_json(strFileName, pt);
                iRet=1;
                ReturnVal (V_BOOL,&iRet);
                }
            catch (std::exception &e) {
                strLastError=e.what();

                ReturnVal (V_UNDEF,&iRet);
                }
            catch (...) {
                ReturnVal (V_BOOL,&iRet);
                }
            }
        else {
            RslError("param 1 must be string");
            ReturnVal (V_BOOL,&iRet);
            }
        return 0;
        }

    RSL_METHOD_DECL(Get) {
        strLastError="";
        VALUE *vFile;
        int iRet=0;
        if (GetParm (1,&vFile) && vFile->v_type == V_STRING) {
            std::string strFileName(vFile->value.string);
            OemToCharBuff((char *)strFileName.data(),(char *)strFileName.data(),strFileName.size());
            try {
                std::string sRet = pt.get<std::string>(strFileName);
                
                //создать переменную в стеке
                VALUE * pv=PushValue(NULL);
                //присвоить ей очередной найденный элемент
                ValueSet (pv,V_STRING,(void *)UTF8_to_CP866(sRet).c_str());
                //1251->866
                //CharToOem(pv->value.string, pv->value.string);
                //запихнуть эту возврат
                ReturnVal2 (pv);
                //можно удалить переменную, она больше не нужна
                PopValue();

                }
            catch (std::exception &e) {
                strLastError=e.what();

                ReturnVal (V_UNDEF,&iRet);
                }
            catch (...) {
                ReturnVal (V_UNDEF,&iRet);
                }
            }
        else {
            RslError("param 1 must be string");
            ReturnVal (V_UNDEF,&iRet);
            }


        return 0;
        }

    RSL_METHOD_DECL(Count) {
        strLastError="";
        VALUE *vFile;
        int iRet=0;
        if (GetParm (1,&vFile) && vFile->v_type == V_STRING) {
            std::string strFileName(vFile->value.string);
            OemToCharBuff((char *)strFileName.data(),(char *)strFileName.data(),strFileName.size());
            try {
                iRet = pt.count(strFileName);
                
                ReturnVal (V_INTEGER,&iRet);
                }
            catch (std::exception &e) {
                strLastError=e.what();

                ReturnVal (V_UNDEF,&iRet);
                }
            catch (...) {
                ReturnVal (V_UNDEF,&iRet);
                }
            }
        else {
            RslError("param 1 must be string");
            ReturnVal (V_UNDEF,&iRet);
            }
        return 0;
        }

    RSL_METHOD_DECL(Find) {
        strLastError="";
        it_type=1;
        VALUE *vFile;
        int iRet=0;
        if (GetParm (1,&vFile) && vFile->v_type == V_STRING) {
            std::string strFileName(vFile->value.string);
            OemToCharBuff((char *)strFileName.data(),(char *)strFileName.data(),strFileName.size());
            try {
                it = pt.find(strFileName);
                iRet=!(it==pt.not_found());
                ReturnVal (V_INTEGER,&iRet);
                }
            catch (std::exception &e) {
                // "Error: " << e.what() 
                strLastError=e.what();

                ReturnVal (V_UNDEF,&iRet);
                }
            catch (...) {
                ReturnVal (V_UNDEF,&iRet);
                }
            }
        else {
            RslError("param 1 must be string");
            ReturnVal (V_UNDEF,&iRet);
            }
        return 0;
        }

    RSL_GETPROP_DECL(currentKey)  { 
        if(it_type) {
            if (it==pt.not_found()) { //not_found
                int iRet=0;
                ReturnVal (V_UNDEF,&iRet);
                }
            else {
                //it->first.data();
                ReturnVal (V_STRING,(void *)UTF8_to_CP866(it->first.data()).c_str());
                }
            }
        else {
            if (iti==pt.end()) { 
                int iRet=0;
                ReturnVal (V_UNDEF,&iRet);
                }
            else {
                ReturnVal (V_STRING,(void *)UTF8_to_CP866(iti->first.data()).c_str());
                }
            }

        return 0;
        }

    RSL_GETPROP_DECL(size)  { 
        int iRet=pt.size();
        ReturnVal(V_INTEGER, &iRet);
        return 0;
        }

    RSL_GETPROP_DECL(empty) { 
        int iRet=pt.empty();
        ReturnVal(V_BOOL, &iRet);
        return 0;
        }
        
    RSL_GETPROP_DECL(currentValue)  { 
        if(it_type) {
            if (it==pt.not_found()) { //not_found
                int iRet=0;
                ReturnVal (V_UNDEF,&iRet);
                }
            else {
                ReturnVal (V_STRING,(void *)UTF8_to_CP866(it->second.get_value<std::string>()).c_str());
                }
            }
        else {
            if (iti==pt.end()) { //not_found
                int iRet=0;
                ReturnVal (V_UNDEF,&iRet);
                }
            else {
                ReturnVal (V_STRING,(void *)UTF8_to_CP866(iti->second.get_value<std::string>()).c_str());
                }
            }
        return 0;
        }

    RSL_GETPROP_DECL(currentNode)  { 
        if(it_type) {
            if (it==pt.not_found()) { 
                int iRet=0;
                ReturnVal (V_UNDEF,&iRet);
                return 0;
                }
            }
        else {
            if (iti==pt.end()) { 
                int iRet=0;
                ReturnVal (V_UNDEF,&iRet);
                return 0;
                }
            }

        TGenObject *obj;
        TPropTree *cppObj;
        TGenClass *cls;

        cls = RslFindUniClass     ("TPropTree");
        obj = CreateObjectOfClassEx (cls,NULL,(void**)&cppObj);
        if(it_type) 
            cppObj->setPTree(it->second);
        else
            cppObj->setPTree(iti->second);
        ReturnVal (V_GENOBJ,obj);
        
        return 0;
        }


    RSL_METHOD_DECL(getChild)  { 
        strLastError="";
        VALUE *vFile;
        int iRet=0;
        if (GetParm (1,&vFile) && vFile->v_type == V_STRING) {
            std::string strFileName(vFile->value.string);
            OemToCharBuff((char *)strFileName.data(),(char *)strFileName.data(),strFileName.size());
            try {
                const ptree &child=pt.get_child(strFileName);

                TGenObject *obj;
                TPropTree *cppObj;
                TGenClass *cls;

                cls = RslFindUniClass     ("TPropTree");
                obj = CreateObjectOfClassEx (cls,NULL,(void**)&cppObj);
                //cppObj->initObject (10,"Test string");
                cppObj->setPTree(child);
                ReturnVal (V_GENOBJ,obj);
                }
            catch (std::exception &e) {
                // "Error: " << e.what() 
                strLastError=e.what();

                ReturnVal (V_UNDEF,&iRet);
                }
            catch (...) {
                ReturnVal (V_UNDEF,&iRet);
                }
            }
        else {
            RslError("param 1 must be string");
            ReturnVal (V_UNDEF,&iRet);
            }
        return 0;
        }


    RSL_METHOD_DECL(getChildKeys) {
        strLastError="";
        VALUE *vFile;
        int iRet=0;
        if (GetParm (1,&vFile) && vFile->v_type == V_STRING) {
            std::string strFileName(vFile->value.string);
            OemToCharBuff((char *)strFileName.data(),(char *)strFileName.data(),strFileName.size());
            try {
                TGenObject * aValue=RslTArrayCreate(10, 5);
                int i=0;
                BOOST_FOREACH(ptree::value_type &v, pt.get_child(strFileName)) {
                    //вернуть массив
                    VALUE * vVal=PushValue(NULL);
                    ValueSet (vVal,V_STRING,(void*)UTF8_to_CP866(v.first.data()).c_str());
                    RslTArrayPut(aValue, i++, vVal);
                    PopValue();
                    }
                ValueSet (retVal,V_GENOBJ,(void *)aValue);
                }
            catch (std::exception &e) {
                strLastError=e.what();

                ReturnVal (V_UNDEF,&iRet);
                }
            catch (...) {
                ReturnVal (V_UNDEF,&iRet);
                }
            }
        else {
            RslError("param 1 must be string");
            ReturnVal (V_UNDEF,&iRet);
            }
        return 0;
        }

    RSL_METHOD_DECL(getChildValues) {
        strLastError="";
        VALUE *vFile;
        int iRet=0;
        if (GetParm (1,&vFile) && vFile->v_type == V_STRING) {
            std::string strFileName(vFile->value.string);
            OemToCharBuff((char *)strFileName.data(),(char *)strFileName.data(),strFileName.size());
            try {
                TGenObject * aValue=RslTArrayCreate(10, 5);
                int i=0;
                BOOST_FOREACH(ptree::value_type &v, pt.get_child(strFileName)) {
                    //вернуть массив
                    VALUE * vVal=PushValue(NULL);
                    ValueSet (vVal,V_STRING,(void*)UTF8_to_CP866(v.second.get_value<std::string>()).c_str());
                    RslTArrayPut(aValue, i++, vVal);
                    PopValue();
                    }
                ValueSet (retVal,V_GENOBJ,(void *)aValue);
                }
            catch (std::exception &e) {
                // "Error: " << e.what() 
                strLastError=e.what();

                ReturnVal (V_UNDEF,&iRet);
                }
            catch (...) {
                ReturnVal (V_UNDEF,&iRet);
                }
            }
        else {
            RslError("param 1 must be string");
            ReturnVal (V_UNDEF,&iRet);
            }
        return 0;
        }

    RSL_METHOD_DECL(getValues) {
        strLastError="";
        VALUE *vFile;
        int iRet=0;
        if (GetParm (1,&vFile) && vFile->v_type == V_STRING) {
            std::string strFileName(vFile->value.string);
            OemToCharBuff((char *)strFileName.data(),(char *)strFileName.data(),strFileName.size());
            try {

                TGenObject * aValue=RslTArrayCreate(10, 5);
                int i=0;

                //std::pair < ptree::const_assoc_iterator, ptree::const_assoc_iterator> range= pt.equal_range(strFileName);
                auto range= pt.equal_range(strFileName);

                for (ptree::const_assoc_iterator it2 = range.first; it2 != range.second ; ++it2) {
                    VALUE * vVal=PushValue(NULL);
                    ValueSet (vVal,V_STRING,(void*)UTF8_to_CP866(it2->second.get_value<std::string>()).c_str());
                    RslTArrayPut(aValue, i++, vVal);
                    PopValue();
                    }
                ValueSet (retVal,V_GENOBJ,(void *)aValue);
                }
            catch (std::exception &e) {
                // "Error: " << e.what() 
                strLastError=e.what();

                ReturnVal (V_UNDEF,&iRet);
                }
            catch (...) {
                ReturnVal (V_UNDEF,&iRet);
                }
            }
        else {
            RslError("param 1 must be string");
            ReturnVal (V_UNDEF,&iRet);
            }
        return 0;
        }

    RSL_METHOD_DECL(findFirst) {
        strLastError="";
        it_type=1;
        VALUE *vFile;
        int iRet=0;
        if (GetParm (1,&vFile) && vFile->v_type == V_STRING) {
            std::string strFileName(vFile->value.string);
            try {
                //std::pair < ptree::const_assoc_iterator, ptree::const_assoc_iterator> range= pt.equal_range(strFileName);
                find_range= pt.equal_range(strFileName);
                it = find_range.first;
                if (it!=find_range.second)
                    iRet=1;
                ReturnVal (V_INTEGER,&iRet);
                }
            catch (std::exception &e) {
                // "Error: " << e.what() 
                strLastError=e.what();

                ReturnVal (V_UNDEF,&iRet);
                }
            catch (...) {
                ReturnVal (V_UNDEF,&iRet);
                }
            }
        else {
            RslError("param 1 must be string");
            ReturnVal (V_UNDEF,&iRet);
            }
        return 0;
        }

    RSL_METHOD_DECL(findNext) {
        strLastError="";
        it_type=1;
        int iRet=0;
            try {
                if(++it != find_range.second)
                    iRet=1;
                ReturnVal (V_INTEGER,&iRet);
                }
            catch (std::exception &e) {
                strLastError=e.what();
                ReturnVal (V_UNDEF,&iRet);
                }
            catch (...) {
                ReturnVal (V_UNDEF,&iRet);
                }
        return 0;
        }

    RSL_METHOD_DECL(first) {
        strLastError="";
        it_type=0;
        int iRet=0;
            try {
                iti=pt.begin();
                if(iti != pt.end())
                    iRet=1;
                ReturnVal (V_INTEGER,&iRet);
                }
            catch (std::exception &e) {
                strLastError=e.what();
                ReturnVal (V_UNDEF,&iRet);
                }
            catch (...) {
                ReturnVal (V_UNDEF,&iRet);
                }
        return 0;
        }

    RSL_METHOD_DECL(next) {
        strLastError="";
        it_type=0;
        int iRet=0;
            try {
                if(++iti!= pt.end())
                    iRet=1;
                ReturnVal (V_INTEGER,&iRet);
                }
            catch (std::exception &e) {
                strLastError=e.what();
                ReturnVal (V_UNDEF,&iRet);
                }
            catch (...) {
                ReturnVal (V_UNDEF,&iRet);
                }
        return 0;
        }



    RSL_GETPROP_DECL(lastError)  { 
        //создать переменную в стеке
        VALUE * pv=PushValue(NULL);
        //присвоить ей очередной найденный элемент
        ValueSet (pv,V_STRING,(void *)strLastError.c_str());
        //1251->866
        CharToOem(pv->value.string, pv->value.string);
        //запихнуть эту возврат
        ReturnVal2 (pv);
        //можно удалить переменную, она больше не нужна
        PopValue();

        return 0;
        }


    RSL_GETPROP_DECL(xmlLocale)  { 
        VALUE * pv=PushValue(NULL);
        if (strXmlLocale.empty()) {
            void * r=NULL;
            ValueSet (pv,V_UNDEF,(void *)&r);
            }
        else
            ValueSet (pv,V_STRING,(void *)strXmlLocale.c_str());
        //CharToOem(pv->value.string, pv->value.string);
        ReturnVal2 (pv);
        PopValue();
        return 0;
        }

    RSL_PUTPROP_DECL(xmlLocale) {
        if (newVal->v_type == V_STRING)
            strXmlLocale=std::string (newVal->value.string);
        return 0;
        }


    RSL_METHOD_DECL(clear) {
        pt.clear();
        return 0;
        }


    RSL_INIT_DECL() {
        strLastError="";
        disableToOEM.value.boolval=0;
        }
                  
    private:
        ptree pt;    
        std::string strLastError;
        ptree::const_assoc_iterator it=pt.not_found(); // = pt.find("pi");
        ptree::iterator iti=pt.end(); 
        std::pair < ptree::const_assoc_iterator, ptree::const_assoc_iterator> find_range;
        int it_type=1; // 0 - iti, 1 -it
        VALUE disableToOEM;
        std::string strXmlLocale;
};

TRslParmsInfo prm1 [] = { {V_STRING,0} };


RSL_CLASS_BEGIN(TPropTree)
  RSL_METH_EX    (LoadXml,        -1, V_UNDEF, 0, RSLNP(prm1), prm1)
  RSL_METH_EX    (LoadJson,       -1, V_UNDEF, 0, RSLNP(prm1), prm1)
  RSL_METH_EX    (Get,            -1, V_UNDEF, 0, RSLNP(prm1), prm1)
  RSL_METH_EX    (Find,           -1, V_UNDEF, 0, RSLNP(prm1), prm1)
  RSL_METH_EX    (Count,          -1, V_UNDEF, 0, RSLNP(prm1), prm1)
  RSL_METH_EX    (getChild,       -1, V_UNDEF, 0, RSLNP(prm1), prm1)
  RSL_METH_EX    (getChildKeys,   -1, V_UNDEF, 0, RSLNP(prm1), prm1)
  RSL_METH_EX    (getChildValues, -1, V_UNDEF, 0, RSLNP(prm1), prm1)
  RSL_METH_EX    (getValues,      -1, V_UNDEF, 0, RSLNP(prm1), prm1)
  RSL_METH_EX    (findFirst,      -1, V_UNDEF, 0, RSLNP(prm1), prm1)
  
  RSL_METH       (clear)
  RSL_PROP_METH  (lastError)
  RSL_PROP_METH  (currentKey)
  RSL_PROP_METH  (currentValue)
  RSL_PROP_METH  (currentNode)
  RSL_PROP_METH  (size)
  RSL_PROP_METH  (empty)
  RSL_PROP_METH2 (xmlLocale)
  RSL_METH       (findNext)
  RSL_METH       (first)
  RSL_METH       (next)
  RSL_PROP       (disableToOEM, disableToOEM)
RSL_CLASS_END  



EXP32 void DLMAPI EXP AddModuleObjects (void) {
    //setlocale(LC_ALL,".866");
    RslAddUniClass (TPropTree::TablePtr,true);
    }




