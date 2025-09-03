#pragma once
#include "structs.h"
#include <dlfcn.h>
#include <string>
#include <codecvt>
#include <locale>
#include <vector>
#include <iomanip>
#include <unordered_map>
#include <cstdint>
#include <sstream>
#include "../Include/Dobby/dobby.h"

// ================= typedef Il2Cpp API =================
typedef Il2CppClass *(*class_from_name_t)(const Il2CppImage *assembly, const char *name_space, const char *name);
typedef MethodInfo *(*class_get_method_from_name_t)(Il2CppClass *klass, const char *name, int paramcount);
typedef Il2CppDomain *(*domain_get_t)();
typedef const Il2CppAssembly **(*domain_get_assemblies_t)(const Il2CppDomain *domain, size_t *size);
typedef const Il2CppImage *(*assembly_get_image_t)(const Il2CppAssembly *assembly);
typedef const Il2CppAssembly *(*domain_assembly_open_t)(Il2CppDomain *domain, const char *name);
typedef FieldInfo *(*class_get_field_from_name_t)(Il2CppClass *klass, const char *name);
typedef Il2CppObject *(*il2cpp_object_new_t)(Il2CppClass *klass);
typedef void (*il2cpp_runtime_object_init_t)(Il2CppObject *obj);
typedef Il2CppObject *(*il2cpp_runtime_invoke_t)(MethodInfo *method, void *obj, void **params, Il2CppObject **exc);
typedef Il2CppString *(*il2cpp_string_new_t)(const char *str);
typedef Il2CppString *(*il2cpp_string_new_utf16_t)(const Il2CppChar *text, int len);
typedef const Il2CppChar *(*il2cpp_string_chars_t)(Il2CppString *str);
typedef int (*il2cpp_string_length_t)(Il2CppString *str);
typedef const char* (*il2cpp_method_get_name_t)(void* method);
typedef const char* (*il2cpp_class_get_name_t)(void* klass);
typedef Il2CppClass* (*il2cpp_class_from_type_t)(const Il2CppType* type);
typedef MethodInfo* (*class_get_method_t)(Il2CppClass* klass, void** iter);
typedef Il2CppType* (*il2cpp_method_get_return_type_t)(const MethodInfo* method);
typedef uint32_t (*il2cpp_method_get_flags_t)(MethodInfo* method, uint32_t* flags);
typedef bool (*il2cpp_type_is_byref_t)(const Il2CppType* type);
typedef Il2CppType* (*il2cpp_method_get_param_t)(MethodInfo* method, int32_t index);
typedef int (*il2cpp_method_get_param_count_t)(void* method);
typedef const char* (*method_get_param_name_t)(const MethodInfo* method, int index);
typedef const char* (*il2cpp_property_get_name_t)(PropertyInfo* prop);
typedef MethodInfo* (*il2cpp_property_get_set_method_t)(PropertyInfo* prop);
typedef MethodInfo* (*il2cpp_property_get_get_method_t)(PropertyInfo* prop);
typedef PropertyInfo* (*il2cpp_class_get_properties_t)(Il2CppClass* klass, void* *iter);
typedef Il2CppClass* (*il2cpp_class_get_interfaces_t)(Il2CppClass* klass, void** iter);


typedef int (*il2cpp_field_get_flags_t)(FieldInfo *field);

typedef bool (*il2cpp_class_is_enum_t)(const Il2CppClass* klass);

typedef size_t (*il2cpp_field_get_offset_t)(FieldInfo* field);

typedef void (*il2cpp_field_static_get_value_t)(FieldInfo* field, void* value);
typedef Il2CppType* (*il2cpp_field_get_type_t)(FieldInfo* field);
typedef FieldInfo* (*il2cpp_class_get_fields_t)(Il2CppClass *klass,void* *iter);
typedef const char* (*il2cpp_field_get_name_t)(FieldInfo* field);
typedef bool (*il2cpp_class_is_valuetype_t)(const Il2CppClass* klass);
typedef int (*il2cpp_class_get_flags_t)(const Il2CppClass* klass);
typedef const char* (*il2cpp_class_get_namespace_t)(Il2CppClass *klass);
typedef Il2CppClass* (*il2cpp_class_get_parent_t)(Il2CppClass* klass);
typedef const Il2CppType* (*il2cpp_class_get_type_t)(Il2CppClass* klass);



// IL2CPP function pointer typedefs
typedef Il2CppObject* (*il2cpp_runtime_invoke_t)(MethodInfo* method, void* obj, void** params, Il2CppObject** exc);
typedef void* (*il2cpp_object_unbox_t)(Il2CppObject* obj);
typedef Il2CppObject* (*il2cpp_object_new_t)(Il2CppClass* klass); // You already have this
typedef const Il2CppType* (*class_get_type_t)(Il2CppClass* klass);
typedef Il2CppObject* (*type_get_object_t)(const Il2CppType* type);
typedef Il2CppDomain* (*domain_get_t)();
typedef const Il2CppImage* (*assembly_get_image_t)(const Il2CppAssembly* assembly);
typedef Il2CppClass* (*class_from_name_t)(const Il2CppImage* image, const char* namespaze, const char* name);


// ================= Helper Base =================

static uint64_t il2cpo_base = 0;

class Il2CppHelper {
protected:
    static void *get_il2cpp() {
        static void *mod = nullptr;
        if (!mod) {
            while (!mod) mod = dlopen("libil2cpp.so", RTLD_NOLOAD | RTLD_NOW);
        }
        return mod;
    }
public:
    static void* getIl2CppPublic() {
        return get_il2cpp();
    }
};

// ========== Hooker Dobby ==========
class Tools : Il2CppHelper {
private:
    static MethodInfo *FindMethod(const char *ns, const char *cls, const char *method, int paramCount) {
        auto domain_get = (Il2CppDomain *(*)())dlsym(get_il2cpp(), "il2cpp_domain_get");
        auto get_assemblies = (const Il2CppAssembly **(*)(const Il2CppDomain *, size_t *))dlsym(get_il2cpp(), "il2cpp_domain_get_assemblies");
        auto get_image = (const Il2CppImage *(*)(const Il2CppAssembly *))dlsym(get_il2cpp(), "il2cpp_assembly_get_image");
        auto get_class = (Il2CppClass *(*)(const Il2CppImage *, const char *, const char *))dlsym(get_il2cpp(), "il2cpp_class_from_name");
        auto get_method = (MethodInfo *(*)(Il2CppClass *, const char *, int))dlsym(get_il2cpp(), "il2cpp_class_get_method_from_name");

        Il2CppDomain *domain = domain_get();
        size_t count = 0;
        const Il2CppAssembly **assemblies = get_assemblies(domain, &count);

        for (size_t i = 0; i < count; i++) {
            const Il2CppImage *image = get_image(assemblies[i]);
            if (!image) continue;

            Il2CppClass *klass = get_class(image, ns, cls);
            if (!klass) continue;

            MethodInfo *m = get_method(klass, method, paramCount);
            if (m && m->methodPointer) return m;
        }

        return nullptr;
    }

public:
    static bool Hook(const char *ns, const char *cls, const char *method,
                                 void *replace, void **original, int paramCount = -1) {
        MethodInfo *target = FindMethod(ns, cls, method, paramCount);
        if (!target || !target->methodPointer) return false;
        return DobbyHook((void *)target->methodPointer, replace, original) == RS_SUCCESS;
    }
};
/*
// ========== Hooker Substrate ==========
class HookerSubstrate : Il2CppHelper {
private:
    static MethodInfo *FindMethod(const char *ns, const char *cls, const char *method, int paramCount) {
        auto domain_get = (Il2CppDomain *(*)())dlsym(get_il2cpp(), "il2cpp_domain_get");
        auto get_assemblies = (const Il2CppAssembly **(*)(const Il2CppDomain *, size_t *))dlsym(get_il2cpp(), "il2cpp_domain_get_assemblies");
        auto get_image = (const Il2CppImage *(*)(const Il2CppAssembly *))dlsym(get_il2cpp(), "il2cpp_assembly_get_image");
        auto get_class = (Il2CppClass *(*)(const Il2CppImage *, const char *, const char *))dlsym(get_il2cpp(), "il2cpp_class_from_name");
        auto get_method = (MethodInfo *(*)(Il2CppClass *, const char *, int))dlsym(get_il2cpp(), "il2cpp_class_get_method_from_name");

        Il2CppDomain *domain = domain_get();
        size_t count = 0;
        const Il2CppAssembly **assemblies = get_assemblies(domain, &count);

        for (size_t i = 0; i < count; i++) {
            const Il2CppImage *image = get_image(assemblies[i]);
            if (!image) continue;

            Il2CppClass *klass = get_class(image, ns, cls);
            if (!klass) continue;

            MethodInfo *m = get_method(klass, method, paramCount);
            if (m && m->methodPointer) return m;
        }

        return nullptr;
    }

public:
    static bool HookMethodByName(const char *ns, const char *cls, const char *method,
                                 void *replace, void **original, int paramCount = -1) {
        MethodInfo *target = FindMethod(ns, cls, method, paramCount);
        if (!target || !target->methodPointer) return false;
        MSHookFunction((void *)target->methodPointer, replace, original);
        return true;
    }
};
*/
// ================= Field Wrapper =================
template<typename T>
class Field : FieldInfo, Il2CppHelper {
    bool statik, init, thread_static;
    void *clazz;

    bool CheckStatic() {
        if ((type->attrs & 0x10) == 0) return false;
        if ((type->attrs & 0x40) != 0) return false;
        thread_static = (offset == -1);
        return true;
    }

public:
    Field(FieldInfo *thiz, void *clas = NULL, void *inst = NULL) {
        init = (thiz != NULL);
        if (init) {
            parent = thiz->parent;
            offset = thiz->offset;
            name = thiz->name;
            token = thiz->token;
            type = thiz->type;
            statik = CheckStatic();
            clazz = statik ? nullptr : (inst ? inst : clas);
        }
    }

    DWORD get_offset() { return offset; }

    T get() {
        if (!init || thread_static) return T();
        return statik ? *(T*)((uint64_t)parent->static_fields + offset) : *(T*)((uint64_t)clazz + offset);
    }

    void set(T val) {
        if (!init || thread_static) return;
        if (statik) *(T*)((uint64_t)parent->static_fields + offset) = val;
        else *(T*)((uint64_t)clazz + offset) = val;
    }

    operator T() { return get(); }
    void operator=(T val) { set(val); }
    T operator()() { return get(); }
};

// ================= String Helper =================
class Il2cppStringHelper : Il2CppHelper {
public:
    Il2CppString *New(const char *utf8) {
        auto fn = (il2cpp_string_new_t)dlsym(get_il2cpp(), "il2cpp_string_new");
        return fn ? fn(utf8) : nullptr;
    }

    Il2CppString *NewUtf16(const Il2CppChar *text, int len) {
        auto fn = (il2cpp_string_new_utf16_t)dlsym(get_il2cpp(), "il2cpp_string_new_utf16");
        return fn ? fn(text, len) : nullptr;
    }

    Il2CppString *FromStdString(const std::string &s) { return New(s.c_str()); }

    std::string ToStdString(Il2CppString *str) {
        if (!str) return "";
        auto chars_fn = (il2cpp_string_chars_t)dlsym(get_il2cpp(), "il2cpp_string_chars");
        auto len_fn = (il2cpp_string_length_t)dlsym(get_il2cpp(), "il2cpp_string_length");
        if (!chars_fn || !len_fn) return "";

        const Il2CppChar *chars = chars_fn(str);
        int len = len_fn(str);
        std::u16string u16(chars, chars + len);
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> conv;
        return conv.to_bytes(u16);
    }
};

// ================= LoadClass Helper =================
class LoadClass : Il2CppHelper {
    Il2CppClass *GetClassFromName(const char *name_space, const char *type_name) {
        auto domain_get = (domain_get_t)dlsym(get_il2cpp(), "il2cpp_domain_get");
        auto dom = domain_get();
        if (!dom) return nullptr;

        size_t assemb_count;
        auto domain_get_assemblies = (domain_get_assemblies_t)dlsym(get_il2cpp(), "il2cpp_domain_get_assemblies");
        const Il2CppAssembly **allAssemb = domain_get_assemblies(dom, &assemb_count);

        auto assembly_get_image = (assembly_get_image_t)dlsym(get_il2cpp(), "il2cpp_assembly_get_image");
        auto class_from_name = (class_from_name_t)dlsym(get_il2cpp(), "il2cpp_class_from_name");

        for (int i = 0; i < assemb_count; i++) {
            auto assemb = allAssemb[i];
            auto img = assembly_get_image(assemb);
            if (!img) continue;

            auto klass = class_from_name(img, name_space, type_name);
            if (klass) {
                dll = img;
                return klass;
            }
        }
        return nullptr;
    }

public:
    const Il2CppImage *dll;
    Il2CppClass *thisclass;

    LoadClass(const char *namespaze, const char *clazz) {
        do { thisclass = GetClassFromName(namespaze, clazz); } while (!thisclass);
    }

    LoadClass(const char *namespaze, const char *clazz, const char *dllname) {
        auto domain_assembly_open = (domain_assembly_open_t)dlsym(get_il2cpp(), "il2cpp_domain_assembly_open");
        auto assembly_get_image = (assembly_get_image_t)dlsym(get_il2cpp(), "il2cpp_assembly_get_image");
        auto domain_get = (domain_get_t)dlsym(get_il2cpp(), "il2cpp_domain_get");
        auto class_from_name = (class_from_name_t)dlsym(get_il2cpp(), "il2cpp_class_from_name");

        dll = assembly_get_image(domain_assembly_open(domain_get(), dllname));
        do { thisclass = class_from_name(dll, namespaze, clazz); } while (!thisclass);
    }

    Il2CppObject *CreateNewObject(bool callCtor = true) {
        auto object_new = (il2cpp_object_new_t)dlsym(get_il2cpp(), "il2cpp_object_new");
        auto object_init = (il2cpp_runtime_object_init_t)dlsym(get_il2cpp(), "il2cpp_runtime_object_init");
        if (!thisclass) return nullptr;
        Il2CppObject *obj = object_new(thisclass);
        if (callCtor && obj) object_init(obj);
        return obj;
    }

    Il2CppObject *CreateNewObjectWithCtor(const char *ctorName, int paramCount, void **params) {
        auto object_new = (il2cpp_object_new_t)dlsym(get_il2cpp(), "il2cpp_object_new");
        auto runtime_invoke = (il2cpp_runtime_invoke_t)dlsym(get_il2cpp(), "il2cpp_runtime_invoke");
        if (!thisclass) return nullptr;

        Il2CppObject *obj = object_new(thisclass);
        if (!obj) return nullptr;

        MethodInfo *ctor = GetMethodInfoByName(ctorName, paramCount);
        if (ctor) {
            Il2CppObject *exc = nullptr;
            runtime_invoke(ctor, obj, params, &exc);
        }
        return obj;
    }

    // ================= Field Utils =================
    FieldInfo *GetFieldInfoByName(const char *name) {
        auto fn = (class_get_field_from_name_t)dlsym(get_il2cpp(), "il2cpp_class_get_field_from_name");
        return fn(thisclass, name);
    }

    template<typename T>
    Field<T> GetFieldByName(const char *name, void *instance = nullptr) {
        return Field<T>(GetFieldInfoByName(name), (void*)thisclass, instance);
    }

    DWORD GetFieldOffset(const char *name) { return GetFieldInfoByName(name)->offset; }
    DWORD GetFieldOffset(FieldInfo *filed) { return filed->offset; }

    // ================= Method Utils =================
    MethodInfo *GetMethodInfoByName(const char *name, int paramcount) {
        auto fn = (class_get_method_from_name_t)dlsym(get_il2cpp(), "il2cpp_class_get_method_from_name");
        return thisclass ? fn(thisclass, name, paramcount) : nullptr;
    }

    DWORD GetMethodOffsetByName(const char *name, int paramcount) {
        auto res = GetMethodInfoByName(name, paramcount);
        return res ? (DWORD)(uintptr_t)res->methodPointer : 0;
    }
};

void* MethodOffset(const char* clazz, const char* name, int argsCount)
{
    auto domain_get             = (domain_get_t)dlsym(get_il2cpp(), "il2cpp_domain_get");
    auto domain_get_assemblies  = (domain_get_assemblies_t)dlsym(get_il2cpp(), "il2cpp_domain_get_assemblies");
    auto assembly_get_image     = (assembly_get_image_t)dlsym(get_il2cpp(), "il2cpp_assembly_get_image");
    auto class_from_name        = (class_from_name_t)dlsym(get_il2cpp(), "il2cpp_class_from_name");
    auto class_get_method       = (class_get_method_from_name_t)dlsym(get_il2cpp(), "il2cpp_class_get_method_from_name");

    auto domain = domain_get();
    if (!domain) return nullptr;

    size_t count = 0;
    const Il2CppAssembly** assemblies = domain_get_assemblies(domain, &count);
    if (!assemblies) return nullptr;

    for (size_t i = 0; i < count; i++) {
        const Il2CppImage* img = assembly_get_image(assemblies[i]);
        if (!img) continue;

        // pakai namespace kosong
        Il2CppClass* klass = class_from_name(img, "", clazz);
        if (!klass) continue;

        MethodInfo* method = class_get_method(klass, name, argsCount);
        if (method && method->methodPointer) {
            return (void*)method->methodPointer;
        }
    }
    return nullptr;
}

Il2CppObject* FieldOffset(const char* nameSpace, const char* clazz, const char* fieldName, void* value, size_t valueSize) {
    auto domain_get             = (domain_get_t)dlsym(get_il2cpp(), "il2cpp_domain_get");
    auto domain_get_assemblies  = (domain_get_assemblies_t)dlsym(get_il2cpp(), "il2cpp_domain_get_assemblies");
    auto assembly_get_image     = (assembly_get_image_t)dlsym(get_il2cpp(), "il2cpp_assembly_get_image");
    auto image_get_class_count  = (image_get_class_count_t)dlsym(get_il2cpp(), "il2cpp_image_get_class_count");
    auto image_get_class        = (image_get_class_t)dlsym(get_il2cpp(), "il2cpp_image_get_class");
    auto object_new             = (object_new_t)dlsym(get_il2cpp(), "il2cpp_object_new");
    auto runtime_object_init    = (runtime_object_init_t)dlsym(get_il2cpp(), "il2cpp_runtime_object_init");
    auto class_get_field        = (class_get_field_from_name_t)dlsym(get_il2cpp(), "il2cpp_class_get_field_from_name");
    auto field_get_offset       = (field_get_offset_t)dlsym(get_il2cpp(), "il2cpp_field_get_offset");

    auto domain = domain_get();
    if (!domain) return nullptr;

    size_t asmCount = 0;
    const Il2CppAssembly** assemblies = domain_get_assemblies(domain, &asmCount);
    if (!assemblies) return nullptr;

    for (size_t i = 0; i < asmCount; i++) {
        const Il2CppImage* img = assembly_get_image(assemblies[i]);
        if (!img) continue;

        size_t classCount = image_get_class_count(img);
        for (size_t c = 0; c < classCount; c++) {
            Il2CppClass* klass = image_get_class(img, c);
            if (!klass || !klass->name || !klass->namespaze) continue;

            // Cocokkan namespace + class
            if (strcmp(klass->name, clazz) != 0) continue;
            if (strcmp(klass->namespaze, nameSpace) != 0) continue;

            // Buat object
            Il2CppObject* obj = object_new(klass);
            if (!obj) return nullptr;

            // Panggil ctor default
            runtime_object_init(obj);

            // Ambil field
            FieldInfo* field = class_get_field(klass, fieldName);
            if (!field) return obj;

            int offset = field_get_offset(field);

            // Tulis nilai field
            memcpy((void*)((uintptr_t)obj + offset), value, valueSize);

            return obj;
        }
    }
    return nullptr;
}

Il2CppObject* AutoInstanceSetField(const char* clazz, const char* fieldName, void* value, size_t valueSize) {
    auto domain_get             = (domain_get_t)dlsym(get_il2cpp(), "il2cpp_domain_get");
    auto domain_get_assemblies  = (domain_get_assemblies_t)dlsym(get_il2cpp(), "il2cpp_domain_get_assemblies");
    auto assembly_get_image     = (assembly_get_image_t)dlsym(get_il2cpp(), "il2cpp_assembly_get_image");
    auto class_from_name        = (class_from_name_t)dlsym(get_il2cpp(), "il2cpp_class_from_name");
    auto object_new             = (object_new_t)dlsym(get_il2cpp(), "il2cpp_object_new");
    auto runtime_object_init    = (void(*)(Il2CppObject*))dlsym(get_il2cpp(), "il2cpp_runtime_object_init");
    auto class_get_field        = (class_get_field_from_name_t)dlsym(get_il2cpp(), "il2cpp_class_get_field_from_name");
    auto field_get_offset       = (field_get_offset_t)dlsym(get_il2cpp(), "il2cpp_field_get_offset");

    auto domain = domain_get();
    if (!domain) return nullptr;

    size_t count = 0;
    const Il2CppAssembly** assemblies = domain_get_assemblies(domain, &count);
    if (!assemblies) return nullptr;

    for (size_t i = 0; i < count; i++) {
        const Il2CppImage* img = assembly_get_image(assemblies[i]);
        if (!img) continue;

        Il2CppClass* klass = class_from_name(img, "", clazz);
        if (!klass) continue;

        // bikin instance
        Il2CppObject* obj = object_new(klass);
        if (obj && runtime_object_init) {
            runtime_object_init(obj);
        }

        // cari field
        FieldInfo* field = class_get_field(klass, fieldName);
        if (field) {
            int offset = field_get_offset(field);
            void* addr = (void*)((uintptr_t)obj + offset);
            memcpy(addr, value, valueSize);
        }

        return obj;
    }
    return nullptr;
}

// ================= Array Helper =================
class ArrayHelper : Il2CppHelper {
public:
    template<typename T>
    Il2CppArray* CreateArray(int length) {
        LoadClass arrayClass("System", "Array");
        auto object_new = (il2cpp_object_new_t)dlsym(get_il2cpp(), "il2cpp_object_new");
        
        Il2CppArray* arr = (Il2CppArray*)object_new(arrayClass.thisclass);
        if (arr) {
            arr->max_length = length;
            arr->bounds = nullptr;
        }
        return arr;
    }

    template<typename T>
    T GetArrayElement(Il2CppArray* array, int index) {
        if (!array || index < 0 || index >= array->max_length) {
            return T{};
        }
        T* elements = (T*)((uint8_t*)array + kIl2CppSizeOfArray);
        return elements[index];
    }

    template<typename T>
    void SetArrayElement(Il2CppArray* array, int index, T value) {
        if (!array || index < 0 || index >= array->max_length) {
            return;
        }
        T* elements = (T*)((uint8_t*)array + kIl2CppSizeOfArray);
        elements[index] = value;
    }

    int GetArrayLength(Il2CppArray* array) {
        return array ? array->max_length : 0;
    }

    template<typename T>
    Il2CppArray* CreateArrayFromVector(const std::vector<T>& vec) {
        Il2CppArray* arr = CreateArray<T>(vec.size());
        if (arr) {
            T* elements = (T*)((uint8_t*)arr + kIl2CppSizeOfArray);
            for (size_t i = 0; i < vec.size(); i++) {
                elements[i] = vec[i];
            }
        }
        return arr;
    }

    template<typename T>
    std::vector<T> ArrayToVector(Il2CppArray* array) {
        std::vector<T> result;
        if (!array) return result;
        
        T* elements = (T*)((uint8_t*)array + kIl2CppSizeOfArray);
        result.reserve(array->max_length);
        for (int i = 0; i < array->max_length; i++) {
            result.push_back(elements[i]);
        }
        return result;
    }
};

// ================= Enum Helper =================
class EnumHelper : Il2CppHelper {
    Il2cppStringHelper strHelper;

public:
    template<typename T>
    T ParseEnum(const char* enumTypeName, const char* valueName) {
        LoadClass enumClass("System", "Enum");
        LoadClass targetClass("", enumTypeName);
        
        auto runtime_invoke = (il2cpp_runtime_invoke_t)dlsym(get_il2cpp(), "il2cpp_runtime_invoke");
        MethodInfo* parseMethod = enumClass.GetMethodInfoByName("Parse", 2);
        
        if (!parseMethod) return T{};
        
        Il2CppString* typeStr = strHelper.New(enumTypeName);
        Il2CppString* valueStr = strHelper.New(valueName);
        void* params[2] = {typeStr, valueStr};
        
        Il2CppObject* exc = nullptr;
        Il2CppObject* result = runtime_invoke(parseMethod, nullptr, params, &exc);
        
        if (exc || !result) return T{};
        return *(T*)((uint8_t*)result + sizeof(Il2CppObject));
    }

    template<typename T>
    std::string EnumToString(T enumValue, const char* enumTypeName) {
        LoadClass enumClass("System", "Enum");
        auto runtime_invoke = (il2cpp_runtime_invoke_t)dlsym(get_il2cpp(), "il2cpp_runtime_invoke");
        
        MethodInfo* toStringMethod = enumClass.GetMethodInfoByName("ToString", 0);
        if (!toStringMethod) return "";
        
        // Create boxed enum value
        LoadClass targetClass("", enumTypeName);
        Il2CppObject* boxedEnum = targetClass.CreateNewObject(false);
        if (!boxedEnum) return "";
        
        *(T*)((uint8_t*)boxedEnum + sizeof(Il2CppObject)) = enumValue;
        
        Il2CppObject* exc = nullptr;
        Il2CppString* result = (Il2CppString*)runtime_invoke(toStringMethod, boxedEnum, nullptr, &exc);
        
        if (exc || !result) return "";
        return strHelper.ToStdString(result);
    }

    template<typename T>
    std::vector<T> GetEnumValues(const char* enumTypeName) {
        std::vector<T> values;
        LoadClass enumClass("System", "Enum");
        auto runtime_invoke = (il2cpp_runtime_invoke_t)dlsym(get_il2cpp(), "il2cpp_runtime_invoke");
        
        MethodInfo* getValuesMethod = enumClass.GetMethodInfoByName("GetValues", 1);
        if (!getValuesMethod) return values;
        
        LoadClass typeClass("System", "Type");
        LoadClass targetClass("", enumTypeName);
        
        // Get type object
        MethodInfo* getTypeMethod = typeClass.GetMethodInfoByName("GetType", 1);
        if (!getTypeMethod) return values;
        
        Il2CppString* typeNameStr = strHelper.New(enumTypeName);
        void* getTypeParams[1] = {typeNameStr};
        
        Il2CppObject* exc = nullptr;
        Il2CppObject* typeObj = runtime_invoke(getTypeMethod, nullptr, getTypeParams, &exc);
        if (exc || !typeObj) return values;
        
        // Get enum values array
        void* getValuesParams[1] = {typeObj};
        Il2CppArray* enumArray = (Il2CppArray*)runtime_invoke(getValuesMethod, nullptr, getValuesParams, &exc);
        
        if (exc || !enumArray) return values;
        
        // Extract values
        T* elements = (T*)((uint8_t*)enumArray + kIl2CppSizeOfArray);
        for (int i = 0; i < enumArray->max_length; i++) {
            values.push_back(elements[i]);
        }
        
        return values;
    }

    template<typename T>
    bool IsValidEnumValue(T value, const char* enumTypeName) {
        LoadClass enumClass("System", "Enum");
        auto runtime_invoke = (il2cpp_runtime_invoke_t)dlsym(get_il2cpp(), "il2cpp_runtime_invoke");
        
        MethodInfo* isDefinedMethod = enumClass.GetMethodInfoByName("IsDefined", 2);
        if (!isDefinedMethod) return false;
        
        LoadClass typeClass("System", "Type");
        MethodInfo* getTypeMethod = typeClass.GetMethodInfoByName("GetType", 1);
        if (!getTypeMethod) return false;
        
        Il2CppString* typeNameStr = strHelper.New(enumTypeName);
        void* getTypeParams[1] = {typeNameStr};
        
        Il2CppObject* exc = nullptr;
        Il2CppObject* typeObj = runtime_invoke(getTypeMethod, nullptr, getTypeParams, &exc);
        if (exc || !typeObj) return false;
        
        // Box the enum value
        LoadClass targetClass("", enumTypeName);
        Il2CppObject* boxedValue = targetClass.CreateNewObject(false);
        if (!boxedValue) return false;
        
        *(T*)((uint8_t*)boxedValue + sizeof(Il2CppObject)) = value;
        
        void* isDefinedParams[2] = {typeObj, boxedValue};
        Il2CppObject* result = runtime_invoke(isDefinedMethod, nullptr, isDefinedParams, &exc);
        
        if (exc || !result) return false;
        return *(bool*)((uint8_t*)result + sizeof(Il2CppObject));
    }
};

// ================= MonoBehaviour Helper =================
class MonoBehaviourHelper : Il2CppHelper {
    Il2cppStringHelper strHelper;

    // -------- Cached il2cpp delegates --------
    static il2cpp_runtime_invoke_t runtime_invoke() {
        static auto fn = (il2cpp_runtime_invoke_t)dlsym(get_il2cpp(), "il2cpp_runtime_invoke");
        return fn;
    }
    static il2cpp_object_unbox_t object_unbox() {
        static auto fn = (il2cpp_object_unbox_t)dlsym(get_il2cpp(), "il2cpp_object_unbox");
        return fn;
    }

    // -------- Utilities --------
    static bool UnboxBool(Il2CppObject* boxed, bool defaultVal=false) {
        if (!boxed) return defaultVal;
        auto unbox = object_unbox();
        if (!unbox) return defaultVal;
        return *(bool*)unbox(boxed);
    }

    // Try resolve System.Type from string:
    // 1) System.Type.GetType("Namespace.Type, Assembly") / "Namespace.Type"
    // 2) Fallback: parse "Namespace.Type" -> {ns, name} then scan all assemblies
    Il2CppObject* ResolveTypeObject(const char* typeName) {
        // Cache MethodInfo for Type.GetType(string)
        static MethodInfo* getTypeMI = []() -> MethodInfo* {
            LoadClass typeClass("System", "Type");
            return typeClass.GetMethodInfoByName("GetType", 1);
        }();
        if (!getTypeMI) return nullptr;

        Il2CppString* typeNameStr = strHelper.New(typeName);
        void* args[1] = { typeNameStr };
        Il2CppObject* exc = nullptr;
        Il2CppObject* typeObj = runtime_invoke()(getTypeMI, nullptr, args, &exc);
        if (!exc && typeObj) return typeObj;

        // fallback: scan all assemblies using LoadClass::GetClassFromName
        // parse "Namespace.Type" into ns + name (last '.')
        std::string full(typeName ? typeName : "");
        std::string ns, name;
        auto pos = full.rfind('.');
        if (pos != std::string::npos) {
            ns = full.substr(0, pos);
            name = full.substr(pos + 1);
        } else {
            // no namespace provided; we can’t reliably find it — bail
            return nullptr;
        }

        // Walk all assemblies and try class_from_name
        auto domain_get = (domain_get_t)dlsym(get_il2cpp(), "il2cpp_domain_get");
        auto domain_get_assemblies = (domain_get_assemblies_t)dlsym(get_il2cpp(), "il2cpp_domain_get_assemblies");
        auto assembly_get_image = (assembly_get_image_t)dlsym(get_il2cpp(), "il2cpp_assembly_get_image");
        auto class_from_name = (class_from_name_t)dlsym(get_il2cpp(), "il2cpp_class_from_name");
        auto class_get_type = (class_get_type_t)dlsym(get_il2cpp(), "il2cpp_class_get_type");
        auto type_get_object = (type_get_object_t)dlsym(get_il2cpp(), "il2cpp_type_get_object");

        if (!domain_get || !domain_get_assemblies || !assembly_get_image || !class_from_name || !class_get_type || !type_get_object)
            return nullptr;

        size_t count = 0;
        const Il2CppAssembly** asms = domain_get_assemblies(domain_get(), &count);
        for (size_t i = 0; i < count; ++i) {
            const Il2CppImage* img = assembly_get_image(asms[i]);
            if (!img) continue;
            Il2CppClass* klass = class_from_name(img, ns.c_str(), name.c_str());
            if (!klass) continue;

            const Il2CppType* t = class_get_type(klass);
            if (!t) continue;
            Il2CppObject* tobj = type_get_object(t);
            if (tobj) return tobj;
        }
        return nullptr;
    }

public:
    // ---------------- UnityEngine.Object / Type helpers ----------------

    Il2CppObject* FindObjectOfType(const char* typeName) {
        static MethodInfo* mi = []() -> MethodInfo* {
            LoadClass objectClass("UnityEngine", "Object");
            return objectClass.GetMethodInfoByName("FindObjectOfType", 1);
        }();
        if (!mi) return nullptr;

        Il2CppObject* typeObj = ResolveTypeObject(typeName);
        if (!typeObj) return nullptr;

        void* args[1] = { typeObj };
        Il2CppObject* exc = nullptr;
        return runtime_invoke()(mi, nullptr, args, &exc), (exc ? nullptr : (Il2CppObject*)runtime_invoke()(mi, nullptr, args, &exc));
    }

    Il2CppArray* FindObjectsOfType(const char* typeName) {
        static MethodInfo* mi = []() -> MethodInfo* {
            LoadClass objectClass("UnityEngine", "Object");
            return objectClass.GetMethodInfoByName("FindObjectsOfType", 1);
        }();
        if (!mi) return nullptr;

        Il2CppObject* typeObj = ResolveTypeObject(typeName);
        if (!typeObj) return nullptr;

        void* args[1] = { typeObj };
        Il2CppObject* exc = nullptr;
        Il2CppArray* res = (Il2CppArray*)runtime_invoke()(mi, nullptr, args, &exc);
        return exc ? nullptr : res;
    }

    // ---------------- GameObject tag queries ----------------

    Il2CppObject* FindGameObjectWithTag(const std::string& tag) {
        static MethodInfo* mi = []() -> MethodInfo* {
            LoadClass go("UnityEngine", "GameObject");
            return go.GetMethodInfoByName("FindWithTag", 1);
        }();
        if (!mi) return nullptr;

        Il2CppString* tagStr = strHelper.New(tag.c_str());
        void* args[1] = { tagStr };
        Il2CppObject* exc = nullptr;
        Il2CppObject* res = runtime_invoke()(mi, nullptr, args, &exc);
        return exc ? nullptr : res;
    }

    Il2CppArray* FindGameObjectsWithTag(const std::string& tag) {
        static MethodInfo* mi = []() -> MethodInfo* {
            LoadClass go("UnityEngine", "GameObject");
            return go.GetMethodInfoByName("FindGameObjectsWithTag", 1);
        }();
        if (!mi) return nullptr;

        Il2CppString* tagStr = strHelper.New(tag.c_str());
        void* args[1] = { tagStr };
        Il2CppObject* exc = nullptr;
        Il2CppArray* res = (Il2CppArray*)runtime_invoke()(mi, nullptr, args, &exc);
        return exc ? nullptr : res;
    }

    // ---------------- Component access by type name ----------------

    Il2CppObject* GetComponent(Il2CppObject* gameObject, const char* componentType) {
        if (!gameObject) return nullptr;

        static MethodInfo* mi = []() -> MethodInfo* {
            LoadClass go("UnityEngine", "GameObject");
            return go.GetMethodInfoByName("GetComponent", 1);
        }();
        if (!mi) return nullptr;

        Il2CppObject* typeObj = ResolveTypeObject(componentType);
        if (!typeObj) return nullptr;

        void* args[1] = { typeObj };
        Il2CppObject* exc = nullptr;
        Il2CppObject* res = runtime_invoke()(mi, gameObject, args, &exc);
        return exc ? nullptr : res;
    }

    Il2CppArray* GetComponents(Il2CppObject* gameObject, const char* componentType) {
        if (!gameObject) return nullptr;

        static MethodInfo* mi = []() -> MethodInfo* {
            LoadClass go("UnityEngine", "GameObject");
            return go.GetMethodInfoByName("GetComponents", 1);
        }();
        if (!mi) return nullptr;

        Il2CppObject* typeObj = ResolveTypeObject(componentType);
        if (!typeObj) return nullptr;

        void* args[1] = { typeObj };
        Il2CppObject* exc = nullptr;
        Il2CppArray* res = (Il2CppArray*)runtime_invoke()(mi, gameObject, args, &exc);
        return exc ? nullptr : res;
    }

    Il2CppObject* AddComponent(Il2CppObject* gameObject, const char* componentType) {
        if (!gameObject) return nullptr;

        static MethodInfo* mi = []() -> MethodInfo* {
            LoadClass go("UnityEngine", "GameObject");
            return go.GetMethodInfoByName("AddComponent", 1);
        }();
        if (!mi) return nullptr;

        Il2CppObject* typeObj = ResolveTypeObject(componentType);
        if (!typeObj) return nullptr;

        void* args[1] = { typeObj };
        Il2CppObject* exc = nullptr;
        Il2CppObject* res = runtime_invoke()(mi, gameObject, args, &exc);
        return exc ? nullptr : res;
    }

    // ---------------- Destroy helpers ----------------

    void DestroyObject(Il2CppObject* obj) {
        if (!obj) return;
        static MethodInfo* mi = []() -> MethodInfo* {
            LoadClass o("UnityEngine", "Object");
            return o.GetMethodInfoByName("Destroy", 1);
        }();
        if (!mi) return;
        void* args[1] = { obj };
        Il2CppObject* exc = nullptr;
        (void)runtime_invoke()(mi, nullptr, args, &exc);
    }

    void DestroyImmediate(Il2CppObject* obj) {
        if (!obj) return;
        static MethodInfo* mi = []() -> MethodInfo* {
            LoadClass o("UnityEngine", "Object");
            return o.GetMethodInfoByName("DestroyImmediate", 1);
        }();
        if (!mi) return;
        void* args[1] = { obj };
        Il2CppObject* exc = nullptr;
        (void)runtime_invoke()(mi, nullptr, args, &exc);
    }

    // ---------------- Behaviour / Component props ----------------

    bool IsActiveAndEnabled(Il2CppObject* monoBehaviour) {
        if (!monoBehaviour) return false;
        static MethodInfo* mi = []() -> MethodInfo* {
            LoadClass b("UnityEngine", "Behaviour");
            return b.GetMethodInfoByName("get_isActiveAndEnabled", 0);
        }();
        if (!mi) return false;
        Il2CppObject* exc = nullptr;
        Il2CppObject* boxed = runtime_invoke()(mi, monoBehaviour, nullptr, &exc);
        return (!exc) && UnboxBool(boxed, false);
    }

    void SetEnabled(Il2CppObject* behaviour, bool enabled) {
        if (!behaviour) return;
        static MethodInfo* mi = []() -> MethodInfo* {
            LoadClass b("UnityEngine", "Behaviour");
            return b.GetMethodInfoByName("set_enabled", 1);
        }();
        if (!mi) return;
        void* args[1] = { &enabled };
        Il2CppObject* exc = nullptr;
        (void)runtime_invoke()(mi, behaviour, args, &exc);
    }

    Il2CppObject* GetGameObject(Il2CppObject* component) {
        if (!component) return nullptr;
        static MethodInfo* mi = []() -> MethodInfo* {
            LoadClass c("UnityEngine", "Component");
            return c.GetMethodInfoByName("get_gameObject", 0);
        }();
        if (!mi) return nullptr;
        Il2CppObject* exc = nullptr;
        Il2CppObject* res = runtime_invoke()(mi, component, nullptr, &exc);
        return exc ? nullptr : res;
    }

    Il2CppObject* GetTransform(Il2CppObject* component) {
        if (!component) return nullptr;
        static MethodInfo* mi = []() -> MethodInfo* {
            LoadClass c("UnityEngine", "Component");
            return c.GetMethodInfoByName("get_transform", 0);
        }();
        if (!mi) return nullptr;
        Il2CppObject* exc = nullptr;
        Il2CppObject* res = runtime_invoke()(mi, component, nullptr, &exc);
        return exc ? nullptr : res;
    }

    // ---------------- MonoBehaviour Invoke API ----------------

    void Invoke(Il2CppObject* monoBehaviour, const std::string& methodName, float time) {
        if (!monoBehaviour) return;
        static MethodInfo* mi = []() -> MethodInfo* {
            LoadClass mb("UnityEngine", "MonoBehaviour");
            return mb.GetMethodInfoByName("Invoke", 2);
        }();
        if (!mi) return;
        Il2CppString* nameStr = strHelper.New(methodName.c_str());
        void* args[2] = { nameStr, &time };
        Il2CppObject* exc = nullptr;
        (void)runtime_invoke()(mi, monoBehaviour, args, &exc);
    }

    void InvokeRepeating(Il2CppObject* monoBehaviour, const std::string& methodName, float time, float repeatRate) {
        if (!monoBehaviour) return;
        static MethodInfo* mi = []() -> MethodInfo* {
            LoadClass mb("UnityEngine", "MonoBehaviour");
            return mb.GetMethodInfoByName("InvokeRepeating", 3);
        }();
        if (!mi) return;
        Il2CppString* nameStr = strHelper.New(methodName.c_str());
        void* args[3] = { nameStr, &time, &repeatRate };
        Il2CppObject* exc = nullptr;
        (void)runtime_invoke()(mi, monoBehaviour, args, &exc);
    }

    void CancelInvoke(Il2CppObject* monoBehaviour, const std::string& methodName = "") {
        if (!monoBehaviour) return;
        if (methodName.empty()) {
            static MethodInfo* mi0 = []() -> MethodInfo* {
                LoadClass mb("UnityEngine", "MonoBehaviour");
                return mb.GetMethodInfoByName("CancelInvoke", 0);
            }();
            if (!mi0) return;
            Il2CppObject* exc = nullptr;
            (void)runtime_invoke()(mi0, monoBehaviour, nullptr, &exc);
        } else {
            static MethodInfo* mi1 = []() -> MethodInfo* {
                LoadClass mb("UnityEngine", "MonoBehaviour");
                return mb.GetMethodInfoByName("CancelInvoke", 1);
            }();
            if (!mi1) return;
            Il2CppString* nameStr = strHelper.New(methodName.c_str());
            void* args[1] = { nameStr };
            Il2CppObject* exc = nullptr;
            (void)runtime_invoke()(mi1, monoBehaviour, args, &exc);
        }
    }

    bool IsInvoking(Il2CppObject* monoBehaviour, const std::string& methodName = "") {
        if (!monoBehaviour) return false;
        if (methodName.empty()) {
            static MethodInfo* mi0 = []() -> MethodInfo* {
                LoadClass mb("UnityEngine", "MonoBehaviour");
                return mb.GetMethodInfoByName("IsInvoking", 0);
            }();
            if (!mi0) return false;
            Il2CppObject* exc = nullptr;
            Il2CppObject* boxed = runtime_invoke()(mi0, monoBehaviour, nullptr, &exc);
            return (!exc) && UnboxBool(boxed, false);
        } else {
            static MethodInfo* mi1 = []() -> MethodInfo* {
                LoadClass mb("UnityEngine", "MonoBehaviour");
                return mb.GetMethodInfoByName("IsInvoking", 1);
            }();
            if (!mi1) return false;
            Il2CppString* nameStr = strHelper.New(methodName.c_str());
            void* args[1] = { nameStr };
            Il2CppObject* exc = nullptr;
            Il2CppObject* boxed = runtime_invoke()(mi1, monoBehaviour, args, &exc);
            return (!exc) && UnboxBool(boxed, false);
        }
    }

    // ---------------- Object alive/active checks ----------------

   static bool IsObjectAlive(Il2CppObject* obj) {
        if (!obj) return false;
        static MethodInfo* mi = []() -> MethodInfo* {
            LoadClass o("UnityEngine", "Object");
            return o.GetMethodInfoByName("op_Implicit", 1);
        }();
        if (!mi) return false;
        void* args[1] = { obj };
        Il2CppObject* exc = nullptr;
        Il2CppObject* boxed = runtime_invoke()(mi, nullptr, args, &exc);
        return (!exc) && UnboxBool(boxed, false);
    }
    
    static bool IsObjectDestroyed(Il2CppObject* obj) {
        return !IsObjectAlive(obj);
    }

    bool IsGameObjectActive(Il2CppObject* gameObject) {
        if (!IsObjectAlive(gameObject)) return false;
        static MethodInfo* mi = []() -> MethodInfo* {
            LoadClass go("UnityEngine", "GameObject");
            return go.GetMethodInfoByName("get_activeSelf", 0);
        }();
        if (!mi) return false;
        Il2CppObject* exc = nullptr;
        Il2CppObject* boxed = runtime_invoke()(mi, gameObject, nullptr, &exc);
        return (!exc) && UnboxBool(boxed, false);
    }

    bool IsGameObjectActiveInHierarchy(Il2CppObject* gameObject) {
        if (!IsObjectAlive(gameObject)) return false;
        static MethodInfo* mi = []() -> MethodInfo* {
            LoadClass go("UnityEngine", "GameObject");
            return go.GetMethodInfoByName("get_activeInHierarchy", 0);
        }();
        if (!mi) return false;
        Il2CppObject* exc = nullptr;
        Il2CppObject* boxed = runtime_invoke()(mi, gameObject, nullptr, &exc);
        return (!exc) && UnboxBool(boxed, false);
    }
};

// ================= GameObject Helper =================
class GameObjectHelper : Il2CppHelper {
    Il2cppStringHelper strHelper;

public:
    // ================= Creation & Destruction =================
    Il2CppObject* CreateGameObject(const std::string& name = "") {
        LoadClass gameObjectClass("UnityEngine", "GameObject");
        
        if (name.empty()) {
            return gameObjectClass.CreateNewObject();
        } else {
            Il2CppString* nameStr = strHelper.New(name.c_str());
            void* params[1] = {nameStr};
            return gameObjectClass.CreateNewObjectWithCtor(".ctor", 1, params);
        }
    }

    Il2CppObject* CreatePrimitive(int primitiveType) {
        LoadClass gameObjectClass("UnityEngine", "GameObject");
        MethodInfo* createPrimitiveMethod = gameObjectClass.GetMethodInfoByName("CreatePrimitive", 1);
        if (!createPrimitiveMethod) return nullptr;

        void* params[1] = {&primitiveType};
        auto runtime_invoke = (il2cpp_runtime_invoke_t)dlsym(get_il2cpp(), "il2cpp_runtime_invoke");
        Il2CppObject* exc = nullptr;
        Il2CppObject* result = runtime_invoke(createPrimitiveMethod, nullptr, params, &exc);
        return exc ? nullptr : result;
    }

    void DestroyGameObject(Il2CppObject* gameObject) {
        if (!gameObject) return;

        LoadClass objectClass("UnityEngine", "Object");
        MethodInfo* destroyMethod = objectClass.GetMethodInfoByName("Destroy", 1);
        if (!destroyMethod) return;

        void* params[1] = {gameObject};
        auto runtime_invoke = (il2cpp_runtime_invoke_t)dlsym(get_il2cpp(), "il2cpp_runtime_invoke");
        Il2CppObject* exc = nullptr;
        runtime_invoke(destroyMethod, nullptr, params, &exc);
    }

    void DestroyGameObjectImmediate(Il2CppObject* gameObject) {
        if (!gameObject) return;

        LoadClass objectClass("UnityEngine", "Object");
        MethodInfo* destroyMethod = objectClass.GetMethodInfoByName("DestroyImmediate", 1);
        if (!destroyMethod) return;

        void* params[1] = {gameObject};
        auto runtime_invoke = (il2cpp_runtime_invoke_t)dlsym(get_il2cpp(), "il2cpp_runtime_invoke");
        Il2CppObject* exc = nullptr;
        runtime_invoke(destroyMethod, nullptr, params, &exc);
    }

    Il2CppObject* DontDestroyOnLoad(Il2CppObject* gameObject) {
        if (!gameObject) return nullptr;

        LoadClass objectClass("UnityEngine", "Object");
        MethodInfo* dontDestroyMethod = objectClass.GetMethodInfoByName("DontDestroyOnLoad", 1);
        if (!dontDestroyMethod) return nullptr;

        void* params[1] = {gameObject};
        auto runtime_invoke = (il2cpp_runtime_invoke_t)dlsym(get_il2cpp(), "il2cpp_runtime_invoke");
        Il2CppObject* exc = nullptr;
        runtime_invoke(dontDestroyMethod, nullptr, params, &exc);
        return exc ? nullptr : gameObject;
    }

    // ================= Find Operations =================
    Il2CppObject* FindGameObject(const std::string& name) {
        LoadClass gameObjectClass("UnityEngine", "GameObject");
        MethodInfo* findMethod = gameObjectClass.GetMethodInfoByName("Find", 1);
        if (!findMethod) return nullptr;

        Il2CppString* nameStr = strHelper.New(name.c_str());
        void* params[1] = {nameStr};

        auto runtime_invoke = (il2cpp_runtime_invoke_t)dlsym(get_il2cpp(), "il2cpp_runtime_invoke");
        Il2CppObject* exc = nullptr;
        Il2CppObject* result = runtime_invoke(findMethod, nullptr, params, &exc);
        return exc ? nullptr : result;
    }

    Il2CppObject* FindWithTag(const std::string& tag) {
        LoadClass gameObjectClass("UnityEngine", "GameObject");
        MethodInfo* findMethod = gameObjectClass.GetMethodInfoByName("FindWithTag", 1);
        if (!findMethod) return nullptr;

        Il2CppString* tagStr = strHelper.New(tag.c_str());
        void* params[1] = {tagStr};

        auto runtime_invoke = (il2cpp_runtime_invoke_t)dlsym(get_il2cpp(), "il2cpp_runtime_invoke");
        Il2CppObject* exc = nullptr;
        Il2CppObject* result = runtime_invoke(findMethod, nullptr, params, &exc);
        return exc ? nullptr : result;
    }

    Il2CppArray* FindGameObjectsWithTag(const std::string& tag) {
        LoadClass gameObjectClass("UnityEngine", "GameObject");
        MethodInfo* findMethod = gameObjectClass.GetMethodInfoByName("FindGameObjectsWithTag", 1);
        if (!findMethod) return nullptr;

        Il2CppString* tagStr = strHelper.New(tag.c_str());
        void* params[1] = {tagStr};

        auto runtime_invoke = (il2cpp_runtime_invoke_t)dlsym(get_il2cpp(), "il2cpp_runtime_invoke");
        Il2CppObject* exc = nullptr;
        Il2CppArray* result = (Il2CppArray*)runtime_invoke(findMethod, nullptr, params, &exc);
        return exc ? nullptr : result;
    }

    // ================= Properties =================
    std::string GetName(Il2CppObject* gameObject) {
        if (!gameObject) return "";

        LoadClass objectClass("UnityEngine", "Object");
        MethodInfo* getNameMethod = objectClass.GetMethodInfoByName("get_name", 0);
        if (!getNameMethod) return "";

        auto runtime_invoke = (il2cpp_runtime_invoke_t)dlsym(get_il2cpp(), "il2cpp_runtime_invoke");
        Il2CppObject* exc = nullptr;
        Il2CppString* result = (Il2CppString*)runtime_invoke(getNameMethod, gameObject, nullptr, &exc);
        return (exc || !result) ? "" : strHelper.ToStdString(result);
    }

    void SetName(Il2CppObject* gameObject, const std::string& name) {
        if (!gameObject) return;

        LoadClass objectClass("UnityEngine", "Object");
        MethodInfo* setNameMethod = objectClass.GetMethodInfoByName("set_name", 1);
        if (!setNameMethod) return;

        Il2CppString* nameStr = strHelper.New(name.c_str());
        void* params[1] = {nameStr};

        auto runtime_invoke = (il2cpp_runtime_invoke_t)dlsym(get_il2cpp(), "il2cpp_runtime_invoke");
        Il2CppObject* exc = nullptr;
        runtime_invoke(setNameMethod, gameObject, params, &exc);
    }

    std::string GetTag(Il2CppObject* gameObject) {
        if (!gameObject) return "";

        LoadClass gameObjectClass("UnityEngine", "GameObject");
        MethodInfo* getTagMethod = gameObjectClass.GetMethodInfoByName("get_tag", 0);
        if (!getTagMethod) return "";

        auto runtime_invoke = (il2cpp_runtime_invoke_t)dlsym(get_il2cpp(), "il2cpp_runtime_invoke");
        Il2CppObject* exc = nullptr;
        Il2CppString* result = (Il2CppString*)runtime_invoke(getTagMethod, gameObject, nullptr, &exc);
        return (exc || !result) ? "" : strHelper.ToStdString(result);
    }

    void SetTag(Il2CppObject* gameObject, const std::string& tag) {
        if (!gameObject) return;

        LoadClass gameObjectClass("UnityEngine", "GameObject");
        MethodInfo* setTagMethod = gameObjectClass.GetMethodInfoByName("set_tag", 1);
        if (!setTagMethod) return;

        Il2CppString* tagStr = strHelper.New(tag.c_str());
        void* params[1] = {tagStr};

        auto runtime_invoke = (il2cpp_runtime_invoke_t)dlsym(get_il2cpp(), "il2cpp_runtime_invoke");
        Il2CppObject* exc = nullptr;
        runtime_invoke(setTagMethod, gameObject, params, &exc);
    }

    int GetLayer(Il2CppObject* gameObject) {
        if (!gameObject) return 0;

        LoadClass gameObjectClass("UnityEngine", "GameObject");
        MethodInfo* getLayerMethod = gameObjectClass.GetMethodInfoByName("get_layer", 0);
        if (!getLayerMethod) return 0;

        auto runtime_invoke = (il2cpp_runtime_invoke_t)dlsym(get_il2cpp(), "il2cpp_runtime_invoke");
        Il2CppObject* exc = nullptr;
        Il2CppObject* result = runtime_invoke(getLayerMethod, gameObject, nullptr, &exc);
        return (!exc && result) ? *(int*)((uint8_t*)result + sizeof(Il2CppObject)) : 0;
    }

    void SetLayer(Il2CppObject* gameObject, int layer) {
        if (!gameObject) return;

        LoadClass gameObjectClass("UnityEngine", "GameObject");
        MethodInfo* setLayerMethod = gameObjectClass.GetMethodInfoByName("set_layer", 1);
        if (!setLayerMethod) return;

        void* params[1] = {&layer};
        auto runtime_invoke = (il2cpp_runtime_invoke_t)dlsym(get_il2cpp(), "il2cpp_runtime_invoke");
        Il2CppObject* exc = nullptr;
        runtime_invoke(setLayerMethod, gameObject, params, &exc);
    }

    bool IsStatic(Il2CppObject* gameObject) {
        if (!gameObject) return false;

        LoadClass gameObjectClass("UnityEngine", "GameObject");
        MethodInfo* isStaticMethod = gameObjectClass.GetMethodInfoByName("get_isStatic", 0);
        if (!isStaticMethod) return false;

        auto runtime_invoke = (il2cpp_runtime_invoke_t)dlsym(get_il2cpp(), "il2cpp_runtime_invoke");
        Il2CppObject* exc = nullptr;
        Il2CppObject* result = runtime_invoke(isStaticMethod, gameObject, nullptr, &exc);
        return (!exc && result) ? *(bool*)((uint8_t*)result + sizeof(Il2CppObject)) : false;
    }

    void SetStatic(Il2CppObject* gameObject, bool isStatic) {
        if (!gameObject) return;

        LoadClass gameObjectClass("UnityEngine", "GameObject");
        MethodInfo* setStaticMethod = gameObjectClass.GetMethodInfoByName("set_isStatic", 1);
        if (!setStaticMethod) return;

        void* params[1] = {&isStatic};
        auto runtime_invoke = (il2cpp_runtime_invoke_t)dlsym(get_il2cpp(), "il2cpp_runtime_invoke");
        Il2CppObject* exc = nullptr;
        runtime_invoke(setStaticMethod, gameObject, params, &exc);
    }

    // ================= Active State =================
    bool IsActive(Il2CppObject* gameObject) {
        if (!gameObject) return false;

        LoadClass gameObjectClass("UnityEngine", "GameObject");
        MethodInfo* activeMethod = gameObjectClass.GetMethodInfoByName("get_activeSelf", 0);
        if (!activeMethod) return false;

        auto runtime_invoke = (il2cpp_runtime_invoke_t)dlsym(get_il2cpp(), "il2cpp_runtime_invoke");
        Il2CppObject* exc = nullptr;
        Il2CppObject* result = runtime_invoke(activeMethod, gameObject, nullptr, &exc);
        return (!exc && result) ? *(bool*)((uint8_t*)result + sizeof(Il2CppObject)) : false;
    }

    bool IsActiveInHierarchy(Il2CppObject* gameObject) {
        if (!gameObject) return false;

        LoadClass gameObjectClass("UnityEngine", "GameObject");
        MethodInfo* activeInHierarchyMethod = gameObjectClass.GetMethodInfoByName("get_activeInHierarchy", 0);
        if (!activeInHierarchyMethod) return false;

        auto runtime_invoke = (il2cpp_runtime_invoke_t)dlsym(get_il2cpp(), "il2cpp_runtime_invoke");
        Il2CppObject* exc = nullptr;
        Il2CppObject* result = runtime_invoke(activeInHierarchyMethod, gameObject, nullptr, &exc);
        return (!exc && result) ? *(bool*)((uint8_t*)result + sizeof(Il2CppObject)) : false;
    }

    void SetActive(Il2CppObject* gameObject, bool active) {
        if (!gameObject) return;

        LoadClass gameObjectClass("UnityEngine", "GameObject");
        MethodInfo* setActiveMethod = gameObjectClass.GetMethodInfoByName("SetActive", 1);
        if (!setActiveMethod) return;

        void* params[1] = {&active};
        auto runtime_invoke = (il2cpp_runtime_invoke_t)dlsym(get_il2cpp(), "il2cpp_runtime_invoke");
        Il2CppObject* exc = nullptr;
        runtime_invoke(setActiveMethod, gameObject, params, &exc);
    }

    // ================= Components =================
    Il2CppObject* GetComponent(Il2CppObject* gameObject, const char* componentType) {
        if (!gameObject) return nullptr;

        LoadClass gameObjectClass("UnityEngine", "GameObject");
        MethodInfo* getComponentMethod = gameObjectClass.GetMethodInfoByName("GetComponent", 1);
        if (!getComponentMethod) return nullptr;

        LoadClass typeClass("System", "Type");
        MethodInfo* getTypeMethod = typeClass.GetMethodInfoByName("GetType", 1);
        if (!getTypeMethod) return nullptr;

        Il2CppString* typeNameStr = strHelper.New(componentType);
        void* getTypeParams[1] = {typeNameStr};

        auto runtime_invoke = (il2cpp_runtime_invoke_t)dlsym(get_il2cpp(), "il2cpp_runtime_invoke");
        Il2CppObject* exc = nullptr;
        Il2CppObject* typeObj = runtime_invoke(getTypeMethod, nullptr, getTypeParams, &exc);
        if (exc || !typeObj) return nullptr;

        void* getComponentParams[1] = {typeObj};
        Il2CppObject* result = runtime_invoke(getComponentMethod, gameObject, getComponentParams, &exc);
        return exc ? nullptr : result;
    }

    Il2CppObject* GetComponentInChildren(Il2CppObject* gameObject, const char* componentType, bool includeInactive = false) {
        if (!gameObject) return nullptr;

        LoadClass gameObjectClass("UnityEngine", "GameObject");
        MethodInfo* getComponentMethod = gameObjectClass.GetMethodInfoByName("GetComponentInChildren", 2);
        if (!getComponentMethod) return nullptr;

        LoadClass typeClass("System", "Type");
        MethodInfo* getTypeMethod = typeClass.GetMethodInfoByName("GetType", 1);
        if (!getTypeMethod) return nullptr;

        Il2CppString* typeNameStr = strHelper.New(componentType);
        void* getTypeParams[1] = {typeNameStr};

        auto runtime_invoke = (il2cpp_runtime_invoke_t)dlsym(get_il2cpp(), "il2cpp_runtime_invoke");
        Il2CppObject* exc = nullptr;
        Il2CppObject* typeObj = runtime_invoke(getTypeMethod, nullptr, getTypeParams, &exc);
        if (exc || !typeObj) return nullptr;

        void* getComponentParams[2] = {typeObj, &includeInactive};
        Il2CppObject* result = runtime_invoke(getComponentMethod, gameObject, getComponentParams, &exc);
        return exc ? nullptr : result;
    }

    Il2CppObject* GetComponentInParent(Il2CppObject* gameObject, const char* componentType) {
        if (!gameObject) return nullptr;

        LoadClass gameObjectClass("UnityEngine", "GameObject");
        MethodInfo* getComponentMethod = gameObjectClass.GetMethodInfoByName("GetComponentInParent", 1);
        if (!getComponentMethod) return nullptr;

        LoadClass typeClass("System", "Type");
        MethodInfo* getTypeMethod = typeClass.GetMethodInfoByName("GetType", 1);
        if (!getTypeMethod) return nullptr;

        Il2CppString* typeNameStr = strHelper.New(componentType);
        void* getTypeParams[1] = {typeNameStr};

        auto runtime_invoke = (il2cpp_runtime_invoke_t)dlsym(get_il2cpp(), "il2cpp_runtime_invoke");
        Il2CppObject* exc = nullptr;
        Il2CppObject* typeObj = runtime_invoke(getTypeMethod, nullptr, getTypeParams, &exc);
        if (exc || !typeObj) return nullptr;

        void* getComponentParams[1] = {typeObj};
        Il2CppObject* result = runtime_invoke(getComponentMethod, gameObject, getComponentParams, &exc);
        return exc ? nullptr : result;
    }

    Il2CppArray* GetComponents(Il2CppObject* gameObject, const char* componentType) {
        if (!gameObject) return nullptr;

        LoadClass gameObjectClass("UnityEngine", "GameObject");
        MethodInfo* getComponentsMethod = gameObjectClass.GetMethodInfoByName("GetComponents", 1);
        if (!getComponentsMethod) return nullptr;

        LoadClass typeClass("System", "Type");
        MethodInfo* getTypeMethod = typeClass.GetMethodInfoByName("GetType", 1);
        if (!getTypeMethod) return nullptr;

        Il2CppString* typeNameStr = strHelper.New(componentType);
        void* getTypeParams[1] = {typeNameStr};

        auto runtime_invoke = (il2cpp_runtime_invoke_t)dlsym(get_il2cpp(), "il2cpp_runtime_invoke");
        Il2CppObject* exc = nullptr;
        Il2CppObject* typeObj = runtime_invoke(getTypeMethod, nullptr, getTypeParams, &exc);
        if (exc || !typeObj) return nullptr;

        void* getComponentsParams[1] = {typeObj};
        Il2CppArray* result = (Il2CppArray*)runtime_invoke(getComponentsMethod, gameObject, getComponentsParams, &exc);
        return exc ? nullptr : result;
    }

    Il2CppArray* GetComponentsInChildren(Il2CppObject* gameObject, const char* componentType, bool includeInactive = false) {
        if (!gameObject) return nullptr;

        LoadClass gameObjectClass("UnityEngine", "GameObject");
        MethodInfo* getComponentsMethod = gameObjectClass.GetMethodInfoByName("GetComponentsInChildren", 2);
        if (!getComponentsMethod) return nullptr;

        LoadClass typeClass("System", "Type");
        MethodInfo* getTypeMethod = typeClass.GetMethodInfoByName("GetType", 1);
        if (!getTypeMethod) return nullptr;

        Il2CppString* typeNameStr = strHelper.New(componentType);
        void* getTypeParams[1] = {typeNameStr};

        auto runtime_invoke = (il2cpp_runtime_invoke_t)dlsym(get_il2cpp(), "il2cpp_runtime_invoke");
        Il2CppObject* exc = nullptr;
        Il2CppObject* typeObj = runtime_invoke(getTypeMethod, nullptr, getTypeParams, &exc);
        if (exc || !typeObj) return nullptr;

        void* getComponentsParams[2] = {typeObj, &includeInactive};
        Il2CppArray* result = (Il2CppArray*)runtime_invoke(getComponentsMethod, gameObject, getComponentsParams, &exc);
        return exc ? nullptr : result;
    }

    Il2CppArray* GetComponentsInParent(Il2CppObject* gameObject, const char* componentType, bool includeInactive = false) {
        if (!gameObject) return nullptr;

        LoadClass gameObjectClass("UnityEngine", "GameObject");
        MethodInfo* getComponentsMethod = gameObjectClass.GetMethodInfoByName("GetComponentsInParent", 2);
        if (!getComponentsMethod) return nullptr;

        LoadClass typeClass("System", "Type");
        MethodInfo* getTypeMethod = typeClass.GetMethodInfoByName("GetType", 1);
        if (!getTypeMethod) return nullptr;

        Il2CppString* typeNameStr = strHelper.New(componentType);
        void* getTypeParams[1] = {typeNameStr};

        auto runtime_invoke = (il2cpp_runtime_invoke_t)dlsym(get_il2cpp(), "il2cpp_runtime_invoke");
        Il2CppObject* exc = nullptr;
        Il2CppObject* typeObj = runtime_invoke(getTypeMethod, nullptr, getTypeParams, &exc);
        if (exc || !typeObj) return nullptr;

        void* getComponentsParams[2] = {typeObj, &includeInactive};
        Il2CppArray* result = (Il2CppArray*)runtime_invoke(getComponentsMethod, gameObject, getComponentsParams, &exc);
        return exc ? nullptr : result;
    }

    Il2CppObject* AddComponent(Il2CppObject* gameObject, const char* componentType) {
        if (!gameObject) return nullptr;

        LoadClass gameObjectClass("UnityEngine", "GameObject");
        MethodInfo* addComponentMethod = gameObjectClass.GetMethodInfoByName("AddComponent", 1);
        if (!addComponentMethod) return nullptr;

        LoadClass typeClass("System", "Type");
        MethodInfo* getTypeMethod = typeClass.GetMethodInfoByName("GetType", 1);
        if (!getTypeMethod) return nullptr;

        Il2CppString* typeNameStr = strHelper.New(componentType);
        void* getTypeParams[1] = {typeNameStr};

        auto runtime_invoke = (il2cpp_runtime_invoke_t)dlsym(get_il2cpp(), "il2cpp_runtime_invoke");
        Il2CppObject* exc = nullptr;
        Il2CppObject* typeObj = runtime_invoke(getTypeMethod, nullptr, getTypeParams, &exc);
        if (exc || !typeObj) return nullptr;

        void* addComponentParams[1] = {typeObj};
        Il2CppObject* result = runtime_invoke(addComponentMethod, gameObject, addComponentParams, &exc);
        return exc ? nullptr : result;
    }

    // ================= Transform =================
    Il2CppObject* GetTransform(Il2CppObject* gameObject) {
        if (!gameObject) return nullptr;

        LoadClass gameObjectClass("UnityEngine", "GameObject");
        MethodInfo* getTransformMethod = gameObjectClass.GetMethodInfoByName("get_transform", 0);
        if (!getTransformMethod) return nullptr;

        auto runtime_invoke = (il2cpp_runtime_invoke_t)dlsym(get_il2cpp(), "il2cpp_runtime_invoke");
        Il2CppObject* exc = nullptr;
        Il2CppObject* result = runtime_invoke(getTransformMethod, gameObject, nullptr, &exc);
        return exc ? nullptr : result;
    }
    
    // ================= Utility =================
    bool CompareTag(Il2CppObject* gameObject, const std::string& tag) {
        if (!gameObject) return false;

        LoadClass gameObjectClass("UnityEngine", "GameObject");
        MethodInfo* compareTagMethod = gameObjectClass.GetMethodInfoByName("CompareTag", 1);
        if (!compareTagMethod) return false;

        Il2CppString* tagStr = strHelper.New(tag.c_str());
        void* params[1] = {tagStr};

        auto runtime_invoke = (il2cpp_runtime_invoke_t)dlsym(get_il2cpp(), "il2cpp_runtime_invoke");
        Il2CppObject* exc = nullptr;
        Il2CppObject* result = runtime_invoke(compareTagMethod, gameObject, params, &exc);
        return (!exc && result) ? *(bool*)((uint8_t*)result + sizeof(Il2CppObject)) : false;
    }

    int GetInstanceID(Il2CppObject* gameObject) {
        if (!gameObject) return 0;

        LoadClass objectClass("UnityEngine", "Object");
        MethodInfo* getInstanceIDMethod = objectClass.GetMethodInfoByName("GetInstanceID", 0);
        if (!getInstanceIDMethod) return 0;

        auto runtime_invoke = (il2cpp_runtime_invoke_t)dlsym(get_il2cpp(), "il2cpp_runtime_invoke");
        Il2CppObject* exc = nullptr;
        Il2CppObject* result = runtime_invoke(getInstanceIDMethod, gameObject, nullptr, &exc);
        return (!exc && result) ? *(int*)((uint8_t*)result + sizeof(Il2CppObject)) : 0;
    }
};

// ================= Unity Helper =================
class UnityHelper : Il2CppHelper {
    Il2cppStringHelper strHelper;
    
    template<typename T>
    T InvokeStaticMethod(const char *className, const char *methodName, int paramCount = 0, void **params = nullptr) {
        LoadClass clazz("UnityEngine", className);
        MethodInfo *method = clazz.GetMethodInfoByName(methodName, paramCount);
        if (!method) return T{};

        auto runtime_invoke = (il2cpp_runtime_invoke_t)dlsym(get_il2cpp(), "il2cpp_runtime_invoke");
        Il2CppObject *exc = nullptr;
        auto result = runtime_invoke(method, nullptr, params, &exc);
        return exc ? T{} : *(T*)((uint64_t)result + sizeof(void*));
    }

    template<typename T>
    T InvokeMethod(Il2CppObject *obj, const char *className, const char *methodName, int paramCount = 0, void **params = nullptr) {
        LoadClass clazz("UnityEngine", className);
        MethodInfo *method = clazz.GetMethodInfoByName(methodName, paramCount);
        if (!method || !obj) return T{};

        auto runtime_invoke = (il2cpp_runtime_invoke_t)dlsym(get_il2cpp(), "il2cpp_runtime_invoke");
        Il2CppObject *exc = nullptr;
        auto result = runtime_invoke(method, obj, params, &exc);
        return exc ? T{} : *(T*)((uint64_t)result + sizeof(void*));
    }

public:
    std::string GetUnityVersion() {
        LoadClass appClass("UnityEngine", "Application");
        MethodInfo *method = appClass.GetMethodInfoByName("get_unityVersion", 0);
        if (!method) return "Unknown";

        auto runtime_invoke = (il2cpp_runtime_invoke_t)dlsym(get_il2cpp(), "il2cpp_runtime_invoke");
        Il2CppObject *exc = nullptr;
        Il2CppString *res = (Il2CppString*)runtime_invoke(method, nullptr, nullptr, &exc);
        return strHelper.ToStdString(res);
    }

    std::string GetGameVersion() {
        LoadClass appClass("UnityEngine", "Application");
        MethodInfo *method = appClass.GetMethodInfoByName("get_version", 0);
        if (!method) return "Unknown";

        auto runtime_invoke = (il2cpp_runtime_invoke_t)dlsym(get_il2cpp(), "il2cpp_runtime_invoke");
        Il2CppObject *exc = nullptr;
        Il2CppString *res = (Il2CppString*)runtime_invoke(method, nullptr, nullptr, &exc);
        return strHelper.ToStdString(res);
    }

    void OpenURL(const std::string &url) {
        LoadClass appClass("UnityEngine", "Application");
        MethodInfo *method = appClass.GetMethodInfoByName("OpenURL", 1);
        if (!method) return;

        auto runtime_invoke = (il2cpp_runtime_invoke_t)dlsym(get_il2cpp(), "il2cpp_runtime_invoke");
        Il2CppString *urlStr = strHelper.New(url.c_str());
        void *params[1] = {urlStr};

        Il2CppObject *exc = nullptr;
        runtime_invoke(method, nullptr, params, &exc);
    }

    float GetTimeScale() { return InvokeStaticMethod<float>("Time", "get_timeScale"); }

    void SetTimeScale(float scale) {
        LoadClass timeClass("UnityEngine", "Time");
        MethodInfo *method = timeClass.GetMethodInfoByName("set_timeScale", 1);
        if (!method) return;

        auto runtime_invoke = (il2cpp_runtime_invoke_t)dlsym(get_il2cpp(), "il2cpp_runtime_invoke");
        void *params[1] = {&scale};

        Il2CppObject *exc = nullptr;
        runtime_invoke(method, nullptr, params, &exc);
    }

    Il2CppObject* GetCamera() {
        LoadClass camClass("UnityEngine", "Camera");
        auto runtime_invoke = (il2cpp_runtime_invoke_t)dlsym(get_il2cpp(), "il2cpp_runtime_invoke");

        // Try get_main first
        MethodInfo *getMain = camClass.GetMethodInfoByName("get_main", 0);
        if (getMain) {
            Il2CppObject *exc = nullptr;
            Il2CppObject *res = runtime_invoke(getMain, nullptr, nullptr, &exc);
            if (!exc && res) return res;
        }

        // Fallback to get_current
        MethodInfo *getCurrent = camClass.GetMethodInfoByName("get_current", 0);
        if (getCurrent) {
            Il2CppObject *exc = nullptr;
            Il2CppObject *res = runtime_invoke(getCurrent, nullptr, nullptr, &exc);
            if (!exc && res) return res;
        }

        return nullptr;
    }

    float GetFieldOfView() {
        Il2CppObject* cam = GetCamera();
        return cam ? InvokeMethod<float>(cam, "Camera", "get_fieldOfView") : -1.0f;
    }

    void SetFieldOfView(float fov) {
        Il2CppObject* cam = GetCamera();
        if (!cam) return;

        LoadClass camClass("UnityEngine", "Camera");
        MethodInfo *method = camClass.GetMethodInfoByName("set_fieldOfView", 1);
        if (!method) return;

        auto runtime_invoke = (il2cpp_runtime_invoke_t)dlsym(get_il2cpp(), "il2cpp_runtime_invoke");
        void *params[1] = {&fov};

        Il2CppObject *exc = nullptr;
        runtime_invoke(method, cam, params, &exc);
    }
};

struct MethodResult {
    std::string nspace;
    std::string cname;
    std::string mname;
    int paramCount;
    uintptr_t address;
};

inline std::vector<MethodResult> g_WorldToScreenMethods;
inline std::vector<MethodResult> g_GetPositionMethods;
inline std::vector<MethodResult> g_GetTransformMethods;
inline std::vector<MethodResult> g_GetMainCurrentMethods;

void Scanner() {
    g_WorldToScreenMethods.clear();
    g_GetPositionMethods.clear();
    g_GetTransformMethods.clear();
    g_GetMainCurrentMethods.clear();

    auto domain_get = (domain_get_t)dlsym(get_il2cpp(), "il2cpp_domain_get");
    auto domain_get_assemblies = (domain_get_assemblies_t)dlsym(get_il2cpp(), "il2cpp_domain_get_assemblies");
    auto assembly_get_image = (assembly_get_image_t)dlsym(get_il2cpp(), "il2cpp_assembly_get_image");
    auto image_get_class_count = (image_get_class_count_t)dlsym(get_il2cpp(), "il2cpp_image_get_class_count");
    auto image_get_class = (image_get_class_t)dlsym(get_il2cpp(), "il2cpp_image_get_class");
    auto class_get_name = (class_get_name_t)dlsym(get_il2cpp(), "il2cpp_class_get_name");
    auto class_get_namespace = (class_get_namespace_t)dlsym(get_il2cpp(), "il2cpp_class_get_namespace");
    auto class_get_methods = (class_get_methods_t)dlsym(get_il2cpp(), "il2cpp_class_get_methods");

    size_t assemb_count;
    auto domain = domain_get();
    const Il2CppAssembly** assemblies = domain_get_assemblies(domain, &assemb_count);

    bool foundMain = false, foundCurrent = false;
    MethodResult tmpMain, tmpCurrent;

    for (int i = 0; i < assemb_count; i++) {
        auto image = assembly_get_image(assemblies[i]);
        if (!image) continue;

        int classCount = image_get_class_count(image);
        for (int c = 0; c < classCount; c++) {
            auto klass = image_get_class(image, c);
            if (!klass) continue;

            const char* cname = class_get_name(klass);
            const char* nspace = class_get_namespace(klass);

            void* iter = nullptr;
            MethodInfo* method;
            while ((method = class_get_methods(klass, &iter))) {
                if (!method->name) continue;

                std::string mname = method->name;
                std::string mnameLower = mname;
                std::transform(mnameLower.begin(), mnameLower.end(), mnameLower.begin(), ::tolower);

                MethodResult res;
                res.nspace     = nspace ? nspace : "";
                res.cname      = cname ? cname : "";
                res.mname      = method->name;
                res.paramCount = method->parameters_count;
                res.address    = (uintptr_t)method->methodPointer;

                // === kategori relevan ===
                if (mnameLower.find("worldtoscreen") != std::string::npos) {
                    g_WorldToScreenMethods.push_back(res);
                }
                if (mnameLower.find("get_position") != std::string::npos) {
                    g_GetPositionMethods.push_back(res);
                }
                if (mnameLower.find("get_transform") != std::string::npos) {
                    g_GetTransformMethods.push_back(res);
                }
                if (mnameLower.find("get_main") != std::string::npos) {
                    tmpMain = res;
                    foundMain = true;
                }
                if (mnameLower.find("get_current") != std::string::npos) {
                    tmpCurrent = res;
                    foundCurrent = true;
                }
            }
        }
    }

    // fallback rules get_main / get_current
    if (foundMain) g_GetMainCurrentMethods.push_back(tmpMain);
    if (foundCurrent) g_GetMainCurrentMethods.push_back(tmpCurrent);
}

void *get_Method(const char *str) {
    void *(*il2cpp_resolve_icall_0)(const char *str) = nullptr;
    void *h = nullptr;
    while (!h) h = dlopen(OBFUSCATE("libil2cpp.so"), 4);
    do {
        il2cpp_resolve_icall_0 = (void *(*)(const char *))dlsym(h, OBFUSCATE("il2cpp_resolve_icall"));
    } while (!il2cpp_resolve_icall_0);
    return il2cpp_resolve_icall_0(str);
}

#define InitResolveFunc(x, y) *reinterpret_cast<void **>(&x) = get_Method(y)
#define InitFunc(x, y) if (y != 0) *reinterpret_cast<void **>(&x) = (void *)(y)
#define FieldBN(myfield, type, inst, nameSpacec, clazzz, fieldName, key) Field<type> myfield = (new LoadClass(OBFUSCATE_KEY(nameSpacec, key), OBFUSCATE_KEY(clazzz, key)))->GetFieldByName<type>(OBFUSCATE_KEY(fieldName, key)); myfield.clazz = inst
