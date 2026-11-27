#include "ZeroCallMedium10.hpp"

extern "C" int __stdcall ZeroCallMedium10Probe(
    unsigned char key,
    unsigned char *data,
    int size,
    unsigned char step,
    int blockSize,
    int limit,
    Zc10AnmVm *vm,
    Zc10Vertex28 *verts,
    Zc10Laser *laser,
    const Zc10Float2 *pos,
    const Zc10Float2 *sz,
    float radius,
    Zc10EnemyMgr *mgr,
    int mode) {
    int sum = 0;
    if (mode & 1) {
        sum += (int)Zc10DecryptBlocks(key, data, size, step, blockSize, limit);
    }
    if (mode & 2) {
        sum += (int)Zc10EncryptBlocks(key, data, size, step, blockSize, limit);
    }
    if (mode & 4) {
        sum += Zc10BuildRibbonU(size, vm, verts);
    }
    if (mode & 8) {
        sum += Zc10BuildRibbonV(size, vm, verts);
    }
    if (mode & 16) {
        Zc10Laser localLaser;
        Zc10Laser *lp = (mode & 64) ? &localLaser : laser;
        sum += lp->CheckHitA(pos, radius);
    }
    if (mode & 32) {
        Zc10KillEnemiesInBox(mgr, pos, sz);
    }
    return sum;
}
