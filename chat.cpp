#include "chat.h"

Chat::Chat() {

    data_count = 0;
    data = nullptr;

    NewMemSize(8);
}
void Chat::reg(char _login[LOGINLENGTH], char _pass[], int pass_length) {
    uint* digest = sha1(_pass, pass_length);
    add(_login, digest);
}

bool Chat::login(char _login[LOGINLENGTH], char _pass[], int pass_length) {

    int index, i = 0;
    for (; i < mem_size; i++) {
        index = hash_func(_login, i * i);
        if (data[index].status == CellStatus::free)
            return false;
        else if (data[index].status == CellStatus::engaged
            && !memcmp(_login, data[index].login, LOGINLENGTH))
            break;
    }
    if (i >= mem_size) return false;
    std::cout << "Propbs count: " << i + 1 << std::endl;

    uint* digest = sha1(_pass, pass_length);

    bool cmpHashes = !memcmp(
        data[index].pass_sha1_hash,
        digest,
        SHA1HASHLENGTHBYTES);
    delete[] digest;

    return cmpHashes;
}

void Chat::add(char login[LOGINLENGTH], uint* digest) {
    int index, i = 0;
    for (; i < mem_size; i++) {
        index = hash_func(login, i * i);
        if (data[index].status == CellStatus::free)
            break;
    }
    if (i >= mem_size)
    {
        resize();
        add(login, digest);
    }
    else {
        data[index] = AuthData(login, digest);
        data_count++;
    }
}

void Chat::NewMemSize(int newMemSize) {
    mem_size = newMemSize;
    data = new AuthData[mem_size];
}

int Chat::hash_func(char login[LOGINLENGTH], int step) {
    const float multiply = 3423.2635;
    long sum = 0;
    for (int i = 0; i < LOGINLENGTH; i++) {
        sum += login[i];
    }
    return int((mem_size * (multiply * sum - int(multiply * sum))) + step*step) % mem_size;
}

void Chat::resize() {
    std::cout << "resize()" << endl;
    AuthData* save = data;
    int save_ms = mem_size;

    NewMemSize(mem_size * 2);
    data_count = 0;

    for (int i = 0; i < save_ms; i++) {
        AuthData& old_data = save[i];
        if (old_data.status == CellStatus::engaged) {

            uint* sha_hash_copy = new uint[SHA1HASHLENGTHUINTS];
            memcpy(sha_hash_copy, old_data.pass_sha1_hash, SHA1HASHLENGTHBYTES);

            add(old_data.login, sha_hash_copy);
        }
    }

    delete[] save;
}