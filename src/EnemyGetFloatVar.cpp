#include "EnemyGetFloatVar.hpp"
#include <math.h>

namespace th128 {

float EgfEnemy::GetFloatVar(int varId) {
    switch (varId) {
    case -10000:
        return (float)(g_EgfRng.GetU32() & 0x7FFFFFFFU);
    case -9999:
        return g_EgfRng.GetFloat();
    case -9987:
        return g_EgfRng.GetFloatSigned();
    case -9998:
        return (float)(g_EgfRng.GetFloatSigned() * 3.1415927410125732);
    case -9997:
    case -9977:
        return pos.x;
    case -9996:
    case -9976:
        return pos.y;
    case -9995:
    case -9975:
        return pos2.x;
    case -9994:
    case -9974:
        return pos2.y;
    case -9993:
    case -9973:
        return pos3.x;
    case -9992:
    case -9972:
        return pos3.y;
    case -9991:
        return g_EgfPlayer->pos.x;
    case -9990:
        return g_EgfPlayer->pos.y;
    case -9989:
        return pos.AngleToPlayer();
    case -9988:
        return field143C;
    case -9986:
        return (float)((flags >> 25) & 1);
    case -9985:
        return (float)intRegs[0];
    case -9984:
        return (float)intRegs[1];
    case -9983:
        return (float)intRegs[2];
    case -9982:
        return (float)intRegs[3];
    case -9981:
        return floatRegs[0];
    case -9980:
        return floatRegs[1];
    case -9979:
        return floatRegs[2];
    case -9978:
        return floatRegs[3];
    case -9935:
        return floatRegs[4];
    case -9934:
        return floatRegs[5];
    case -9933:
        return floatRegs[6];
    case -9932:
        return floatRegs[7];
    case -9943:
        return g_EgfEnemyManager->boss != 0 ? (float)g_EgfEnemyManager->boss->intRegs[0] : 0.0f;
    case -9942:
        return g_EgfEnemyManager->boss != 0 ? (float)g_EgfEnemyManager->boss->intRegs[1] : 0.0f;
    case -9941:
        return g_EgfEnemyManager->boss != 0 ? (float)g_EgfEnemyManager->boss->intRegs[2] : 0.0f;
    case -9940:
        return g_EgfEnemyManager->boss != 0 ? (float)g_EgfEnemyManager->boss->intRegs[3] : 0.0f;
    case -9939:
        return g_EgfEnemyManager->boss != 0 ? g_EgfEnemyManager->boss->floatRegs[0] : 0.0f;
    case -9938:
        return g_EgfEnemyManager->boss != 0 ? g_EgfEnemyManager->boss->floatRegs[1] : 0.0f;
    case -9937:
        return g_EgfEnemyManager->boss != 0 ? g_EgfEnemyManager->boss->floatRegs[2] : 0.0f;
    case -9936:
        return g_EgfEnemyManager->boss != 0 ? g_EgfEnemyManager->boss->floatRegs[3] : 0.0f;
    case -9911:
        return g_EgfEnemyManager->boss != 0 ? atan2f(g_EgfEnemyManager->boss->vel.y, g_EgfEnemyManager->boss->vel.x) : 0.0f;
    case -9910:
        return g_EgfEnemyManager->boss != 0 ? g_EgfEnemyManager->boss->angle1 : 0.0f;
    case -9971:
        return speed1;
    case -9970:
        return speed2;
    case -9958:
        return (float)atan2((double)vel.y, (double)vel.x);
    case -9969:
        return angle1;
    case -9968:
        return angle2;
    case -9967:
        return field1250;
    case -9966:
        return field1284;
    case -9965:
        return g_EgfPlayer->pos.x;
    case -9964:
        return g_EgfPlayer->pos.y;
    case -9963:
        return g_EgfEnemyManager->boss != 0 ? g_EgfEnemyManager->boss->pos.x : 0.0f;
    case -9962:
        return g_EgfEnemyManager->boss != 0 ? g_EgfEnemyManager->boss->pos.y : 128.0f;
    case -9960:
        return (float)g_EgfField4B4D30;
    case -9959:
        return (float)g_EgfDifficulty;
    case -9957:
        return 1.0f;
    case -9956:
        return pos2.AngleToPlayer();
    case -9955:
        return pos3.AngleToPlayer();
    case -9954:
        return (float)field5004;
    case -9953:
        return (float)((float)g_EgfDifficulty == 0.0f);
    case -9952:
        return (float)((float)g_EgfDifficulty == 1.0f);
    case -9951:
        return (float)((float)g_EgfDifficulty == 2.0f);
    case -9950:
        return (float)((float)g_EgfDifficulty == 3.0f);
    case -9949:
        return (float)g_EgfEnemyManager->field10;
    case -9948:
        return (float)g_EgfEnemyManager->field14;
    case -9947:
        return (float)g_EgfEnemyManager->field18;
    case -9946:
        return (float)g_EgfEnemyManager->fieldC0;
    case -9945:
        return (float)(g_EgfField4B4CF8 + g_EgfField4B4CF4 * 2);
    case -9944:
        return pos.DistanceTo(&g_EgfPlayer->pos);
    case -9931:
        return (float)g_EgfEnemyManager->fieldC8;
    case -9930:
        return (float)g_EgfSpellCard.GetSomething();
    case -9927:
        return (float)((float)(g_EgfPlayer->field74 == 0) != 0.0f && g_EgfField4D33CC != 0);
    case -9909:
        return (float)field11C4;
    case -9926:
        return (float)g_EgfEnemyManager->intVars[0];
    case -9925:
        return (float)g_EgfEnemyManager->intVars[1];
    case -9924:
        return (float)g_EgfEnemyManager->intVars[2];
    case -9923:
        return (float)g_EgfEnemyManager->intVars[3];
    case -9922:
        return g_EgfEnemyManager->floatVars[0];
    case -9921:
        return g_EgfEnemyManager->floatVars[1];
    case -9920:
        return g_EgfEnemyManager->floatVars[2];
    case -9919:
        return g_EgfEnemyManager->floatVars[3];
    case -9918:
        return g_EgfEnemyManager->floatVars[4];
    case -9917:
        return g_EgfEnemyManager->floatVars[5];
    case -9916:
        return g_EgfEnemyManager->floatVars[6];
    case -9915:
        return g_EgfEnemyManager->floatVars[7];
    case -9914:
        return (float)field11C0;
    default:
        return 0.0f;
    }
}

} // namespace th128
