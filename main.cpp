#include <iostream>
#include <vector>
#include <string>
using namespace std;
enum Instruction
{
    inbox,
    outbox,
    add,
    sub,
    copyto,
    copyfrom,
    jump,
    jumpifzero
};

// 重载符号

istream &operator>>(istream &in, Instruction &ins);

class Level
{
private:
    vector<vector<vector<int>>> levelinfo;
    vector<vector<Instruction>> instruction;
    vector<int (Level::*)(int)> op; // 指令可调用vector

    // 指令执行
    int nins;           // 指令数量
    double inscanshu;   // 指令参数
    Instruction tmpins; // 指令
    bool nownumext;     // 现在手里有没有
    int nownum;
    vector<int> input;
    vector<int> target_output;
    vector<int> output;
    int spacen;
    vector<Instruction> allowed_instruction;
    int ptrinput;
    int insptr;
    vector<bool> assignedspace;
    vector<int> space;
    vector<Instruction> insgroup;
    vector<double> insdata;

protected:
    int Inbox(int i);
    int Outbox(int i);
    int Add(int i);
    int Sub(int i);
    int Copyto(int i);
    int Copyfrom(int i);
    int Jump(int ins);
    int Jumpifzero(int ins);
    bool check(Instruction tmpins, vector<Instruction> allowed_instruction);
    bool result();

public:
    Level();
    int execute(int wlevel);
};

// main函数
int main()
{
    int leveln;
    cin >> leveln;
    Level l;
    l.execute(leveln);
    return 0;
}

istream &operator>>(istream &in, Instruction &ins)
{
    string s;
    in >> s;
    if (s == "inbox")
        ins = inbox;
    else if (s == "outbox")
        ins = outbox;
    else if (s == "add")
        ins = add;
    else if (s == "sub")
        ins = sub;
    else if (s == "copyto")
        ins = copyto;
    else if (s == "copyfrom")
        ins = copyfrom;
    else if (s == "jump")
        ins = jump;
    else
        ins = jumpifzero;
    return in;
}
// 构造
Level::Level()
{
    levelinfo = vector<vector<vector<int>>>(4, vector<vector<int>>(3, vector<int>()));
    instruction = vector<vector<Instruction>>(4, vector<Instruction>());
    op = vector<int (Level::*)(int)>(); // 调用函数
    op.clear();
    op.push_back(&Level::Inbox);
    op.push_back(&Level::Outbox);
    op.push_back(&Level::Add);
    op.push_back(&Level::Sub);
    op.push_back(&Level::Copyto);
    op.push_back(&Level::Copyfrom);
    op.push_back(&Level::Jump);
    op.push_back(&Level::Jumpifzero);

    // 关卡信息
    // 一个三维数组，x轴是第几关;y轴分别是input，targetoutput，空地数，指令;z轴是数据

    // 第一关

    int a1[] = {1, 2};
    levelinfo[0][0].assign(a1, a1 + 2);
    int a2[] = {1, 2};
    levelinfo[0][1].assign(a2, a2 + 2);
    int a3[] = {0};
    levelinfo[0][2].assign(a3, a3 + 1);
    Instruction ins0[] = {inbox, outbox};
    instruction[0].assign(ins0, ins0 + 2);
    // 第二关
    int a4[] = {3, 9, 5, 1, -2, -2, 9, -9};
    levelinfo[1][0].assign(a4, a4 + 8);
    int a5[] = {-6, 6, 4, -4, 0, 0, 18, -18};
    levelinfo[1][1].assign(a5, a5 + 8);
    int a8[] = {3};
    levelinfo[1][2].assign(a8, a8 + 1);
    Instruction ins1[] = {inbox, outbox, add, sub, copyto, copyfrom, jump, jumpifzero};
    instruction[1].assign(ins1, ins1 + 8);
    // 第三关
    int a6[] = {6, 2, 7, 7, -9, 3, -3, -3};
    levelinfo[2][0].assign(a6, a6 + 8);
    int a7[] = {7, -3};
    levelinfo[2][1].assign(a7, a7 + 2);
    int a9[] = {3};
    levelinfo[2][2].assign(a9, a9 + 1);
    instruction[2].assign(ins1, ins1 + 8);
    // 第四关（自定义）
}
// 执行，开始玩游戏
int Level::execute(int wlevel)
{
    // 关卡基础信息设置
    wlevel -= 1; // cout<<"Fail";
    // return 0;
    int numinput = 0;                     // 输入数量
    input = levelinfo[wlevel][0];         // 输入
    target_output = levelinfo[wlevel][1]; // 目标输出
    output.clear();
    spacen = levelinfo[wlevel][2][0];          // 空地数量
    allowed_instruction = instruction[wlevel]; // 允许的操作
    ptrinput = 0;                              // input指针
    insptr = 0;                                // 指令指针
    assignedspace = vector<bool>(spacen);      // 空地上的位置空着吗
    for (int i = 0; i < spacen; i++)
    {
        assignedspace[i] = false;
    }
    space = vector<int>(spacen); // 空地上的数字存储
    nownumext = false;

    cin >> nins;
    insgroup = vector<Instruction>(nins); // 指令数组
    insdata = vector<double>(nins);       // 指令参数数组

    for (int i = 0; i < nins; i++)
    {
        cin >> tmpins;
        insgroup[i] = tmpins;
        if (!(tmpins == inbox || tmpins == outbox))
        {
            cin >> inscanshu;
            insdata[i] = inscanshu;
        }
    }

    while (insptr < nins)
    {
        Instruction tmpins = insgroup[insptr];
        double inscanshu = insdata[insptr];
        if (!check(tmpins, allowed_instruction) || inscanshu != (int)inscanshu)
        {
            cout << "Error on instruction " << insptr + 1;
            return 0;
        }
        else
        {
            if (tmpins == inbox)
            {
                numinput++;
            }
            if (numinput > input.size())
            {
                if (!result())
                    cout << "Fail";
                else
                    cout <<

                        "Success";
                return 0;
            }
            if (((this->*op[tmpins])((int)inscanshu)) == 1)
            {
                cout << "Error on instruction " << insptr + 1;
                return 0;
            }
        }
    }
    if (!result())
        cout << "Fail";
    else
        cout << "Success";
    return 0;
    cout << "Success";
    return 0;
}
bool Level::result()
{
    if (output.size() != target_output.size())
    {
        return false;
    }
    for (int i = 0; i < target_output.size(); i++)
    {
        if (target_output[i] != output[i])
        {
            return false;
        }
    }
    return true;
}
bool Level::check(Instruction tmpins, vector<Instruction> allowed_instruction)
{
    for (int i = 0; i < allowed_instruction.size(); i++)
    {
        if (tmpins == allowed_instruction[i])
        {
            return true;
        }
    }
    return false;
}
int Level::Inbox(int i)
{
    nownum = input[ptrinput++];
    nownumext = true;
    insptr++;
    return 0;
}
int Level::Outbox(int i)
{
    if (!nownumext)
        return 1;
    output.push_back(nownum);
    nownumext = false;
    insptr++;
    return 0;
}
int Level::Add(int i)
{
    if (!nownumext || i < 0 || i >= spacen || !assignedspace[i])
        return 1;
    nownum += space[i];
    insptr++;
    return 0;
}
int Level::Sub(int i)
{
    if (!nownumext || i < 0 || i >= spacen || !assignedspace[i])
        return 1;
    nownum -= space[i];
    insptr++;
    return 0;
}
int Level::Copyto(int i)
{
    if (!nownumext || i >= spacen || i < 0)
        return 1;
    space[i] = nownum;
    assignedspace[i] = true;
    insptr++;
    return 0;
}
int Level::Copyfrom(int i)
{
    if (i >= spacen || !assignedspace[i] || i < 0)
        return 1;
    nownum = space[i];
    nownumext = true;
    insptr++;
    return 0;
}
int Level::Jump(int ins)
{
    if (ins <= 0 || ins > nins)
        return 1;
    insptr = ins - 1;
    return 0;
}
int Level::Jumpifzero(int ins)
{
    if (!nownumext || ins <= 0 || ins > nins)
        return 1;
    if (nownum == 0)
        insptr = ins - 1;
    else
        insptr++;
    return 0;
}
