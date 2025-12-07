#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <ctime>
using namespace std;

// classe base "exercicio"
// representa qualquer tipo de exercício (cardio ou força)
//id, nome, status e metodos virtuais
class Exercicio {
protected:
    int id;
    string nome;
    bool ativo;
    static int proximoId; // controla a geração de ids

public:
    // construtor usado para novos cadastros
    // gera id automaticamente
    Exercicio(string n) {
        nome = n;
        ativo = true;
        id = proximoId++;
    }

    // construtor usado ao ler do arquivo
    // recebe o id fixo que já existia
    Exercicio(int idExistente, string n, bool status) {
        id = idExistente;
        nome = n;
        ativo = status;

        // ajusta o contador de ids global
        if (idExistente >= proximoId)
            proximoId = idExistente + 1;
    }

    virtual ~Exercicio() {}

    int getId() { return id; }
    string getNome() { return nome; }
    bool isAtivo() { return ativo; }
    void desativar() { ativo = false; }

    // métodos polimórficos obrigatórios
    virtual double calcularTempo() = 0;      // calcula tempo total
    virtual double calcularCalorias() = 0;   // calcula calorias gastas
    virtual int getTipo() = 0;               // retorna tipo (1 cardio / 2 força)
    virtual void exibirDetalhes() = 0;       // mostra informações do exercício
    virtual string serializar() = 0;         // converte em linha para salvar em arquivo
};
int Exercicio::proximoId = 1;

// classe cardio
// representa exercícios aeróbicos
// calcula tempo e calorias com base em duração e cal/min
class Cardio : public Exercicio {
private:
    int duracao;
    double calPorMin;

public:
    // construtor normal
    Cardio(string n, int d, double cpm)
        : Exercicio(n), duracao(d), calPorMin(cpm) {}

    // construtor ao ler do arquivo
    Cardio(int idExist, string n, bool st, int d, double cpm)
        : Exercicio(idExist, n, st), duracao(d), calPorMin(cpm) {}

    // retorna tempo total (igual à duração)
    double calcularTempo() override {
        return duracao;
    }

    // calcula calorias = duração * cal/min
    double calcularCalorias() override {
        return duracao * calPorMin;
    }

    int getTipo() override { return 1; }

    // exibe dados do exercício
    void exibirDetalhes() override {
        cout << "[cardio] id=" << id
             << " nome=" << nome
             << " duracao=" << duracao
             << " cal/min=" << calPorMin
             << " ativo=" << ativo << endl;
    }

    // gera linha para salvar no arquivo exercicios.txt
    string serializar() override {
        return "1;" + to_string(id) + ";" + nome + ";" +
               to_string(duracao) + ";" + to_string(calPorMin) + ";" +
               to_string(ativo);
    }
};

// classe forca
// representa exercícios de musculação
// calcula tempo com base em reps/séries/descanso
// calcula calorias pelo volume total levantado
class Forca : public Exercicio {
private:
    double carga;
    int series;
    int repeticoes;
    int descanso;

public:
    // construtor normal
    Forca(string n, double c, int s, int r, int d)
        : Exercicio(n), carga(c), series(s), repeticoes(r), descanso(d) {}

    // construtor ao ler do arquivo
    Forca(int idExist, string n, bool st, double c, int s, int r, int d)
        : Exercicio(idExist, n, st), carga(c), series(s), repeticoes(r), descanso(d) {}

    // calcula tempo = (reps * séries * 3s + descanso total) convertido para minutos
    double calcularTempo() override {
        double tempoRep = series * repeticoes * 3;
        double tempoDesc = series * descanso;
        return (tempoRep + tempoDesc) / 60.0;
    }

    // calcula calorias usando fórmula pedida
    double calcularCalorias() override {
        return series * repeticoes * carga * 0.15;
    }

    int getTipo() override { return 2; }

    // exibe informações completas
    void exibirDetalhes() override {
        cout << "[forca] id=" << id
             << " nome=" << nome
             << " carga=" << carga
             << " series=" << series
             << " reps=" << repeticoes
             << " descanso=" << descanso
             << " ativo=" << ativo << endl;
    }

    // serializa para salvar no arquivo
    string serializar() override {
        return "2;" + to_string(id) + ";" + nome + ";" +
               to_string(carga) + ";" + to_string(series) + ";" +
               to_string(repeticoes) + ";" + to_string(descanso) + ";" +
               to_string(ativo);
    }
};

// classe ficha
// representa um treino composto por vários exercícios
// calcula tempo total e calorias da soma de todos
class Ficha {
public:
    int id;
    string nome;
    vector<Exercicio*> exercicios;

    Ficha(int i, string n) : id(i), nome(n) {}

    // soma o tempo de todos os exercícios
    double tempoTotal() {
        double soma = 0;
        for (auto e : exercicios) soma += e->calcularTempo();
        return soma;
    }

    // soma as calorias de todos os exercícios
    double caloriasTotal() {
        double soma = 0;
        for (auto e : exercicios) soma += e->calcularCalorias();
        return soma;
    }

    // gera linha para salvar em fichas.txt
    string serializar() {
        string linha = to_string(id) + ";" + nome + ";" + to_string(exercicios.size());
        for (auto e : exercicios) linha += ";" + to_string(e->getId());
        return linha;
    }
};

// função que retorna a data/hora atual formatada
// usada para registrar o histórico
string agora() {
    time_t t = time(nullptr);
    tm* dt = localtime(&t);
    char buf[50];
    strftime(buf, 50, "%d/%m/%Y %H:%M:%S", dt);
    return string(buf);
}

// função principal
// carrega arquivos, mostra menu e salva tudo ao sair
int main() {
    vector<Exercicio*> exercicios;
    vector<Ficha> fichas;
    int fichaProxId = 1;

    // leitura do arquivo de exercícios na inicialização
    // reconstrói todos os objetos cardio/força
    ifstream arqEx("exercicios.txt");
    if (arqEx.is_open()) {
        string linha;
        while (getline(arqEx, linha)) {
            stringstream ss(linha);
            vector<string> v;
            string campo;

            while (getline(ss, campo, ';')) v.push_back(campo);
            int tipo = stoi(v[0]);

            if (tipo == 1)
                exercicios.push_back(new Cardio(stoi(v[1]), v[2], stoi(v[5]), stoi(v[3]), stod(v[4])));
            else
                exercicios.push_back(new Forca(stoi(v[1]), v[2], stoi(v[7]), stod(v[3]),
                                               stoi(v[4]), stoi(v[5]), stoi(v[6])));
        }
    }

    // leitura do arquivo de fichas
    // reconstrói fichas e associa exercícios existentes
    ifstream arqFi("fichas.txt");
    if (arqFi.is_open()) {
        string linha;
        while (getline(arqFi, linha)) {

            stringstream ss(linha);
            vector<string> v;
            string campo;

            while (getline(ss, campo, ';')) v.push_back(campo);

            int idFicha = stoi(v[0]);
            string nomeFicha = v[1];
            int qtd = stoi(v[2]);

            Ficha f(idFicha, nomeFicha);

            for (int i = 0; i < qtd; i++) {
                int exId = stoi(v[3 + i]);

                for (auto e : exercicios)
                    if (e->getId() == exId)
                        f.exercicios.push_back(e);
            }

            fichas.push_back(f);

            if (idFicha >= fichaProxId)
                fichaProxId = idFicha + 1;
        }
    }

    // loop principal do menu
    int op;
    do {
        cout << "\n==== menu principal ====\n";
        cout << "1 - gerenciar exercicios\n";
        cout << "2 - gerenciar fichas\n";
        cout << "3 - registrar treino\n";
        cout << "4 - ver historico\n";
        cout << "0 - sair\n";
        cin >> op;

        // menu 1 - gerenciamento de exercícios
        if (op == 1) {
            int op2;
            cout << "\n1 - cadastrar\n2 - listar\n3 - excluir\n";
            cin >> op2;

            // cadastrar novo exercício
            if (op2 == 1) {
                int tipo;
                cout << "\n1-cardio  2-forca\n";
                cin >> tipo;
                cin.ignore();

                string nome;
                cout << "nome: ";
                getline(cin, nome);

                if (tipo == 1) {
                    int d; double cpm;
                    cout << "duracao: "; cin >> d;
                    cout << "cal/min: "; cin >> cpm;
                    exercicios.push_back(new Cardio(nome, d, cpm));
                } else {
                    double carga; int s, r, d;
                    cout << "carga: "; cin >> carga;
                    cout << "series: "; cin >> s;
                    cout << "reps: "; cin >> r;
                    cout << "descanso: "; cin >> d;
                    exercicios.push_back(new Forca(nome, carga, s, r, d));
                }
            }

            // listar exercícios ativos
            if (op2 == 2) {
                for (auto e : exercicios)
                    if (e->isAtivo())
                        e->exibirDetalhes();
            }

            // excluir lógico (não remove do vetor)
            if (op2 == 3) {
                int id; cout << "id: "; cin >> id;
                for (auto e : exercicios)
                    if (e->getId() == id)
                        e->desativar();
            }
        }

        // menu 2 - gerenciamento de fichas
        else if (op == 2) {
            int op2;
            cout << "\n1 - criar ficha\n2 - adicionar exercicio\n3 - listar fichas\n";
            cin >> op2;

            // criar nova ficha
            if (op2 == 1) {
                cin.ignore();
                string n;
                cout << "nome da ficha: ";
                getline(cin, n);
                fichas.push_back(Ficha(fichaProxId++, n));
            }

            // adicionar exercício a uma ficha
            if (op2 == 2) {
                int idF, idE;
                cout << "id da ficha: "; cin >> idF;
                cout << "id do exercicio: "; cin >> idE;

                Exercicio* escolhido = nullptr;
                for (auto e : exercicios)
                    if (e->getId() == idE && e->isAtivo())
                        escolhido = e;

                for (auto& f : fichas)
                    if (f.id == idF && escolhido)
                        f.exercicios.push_back(escolhido);
            }

            // listar fichas completas
            if (op2 == 3) {
                for (auto& f : fichas) {
                    cout << "\nficha " << f.id << " - " << f.nome << endl;
                    for (auto e : f.exercicios)
                        e->exibirDetalhes();

                    cout << "tempo total = " << f.tempoTotal() << endl;
                    cout << "cal total = " << f.caloriasTotal() << endl;
                }
            }
        }

        // menu 3 - registrar execução de ficha
        // salva no historico.txt
        else if (op == 3) {
            int id;
            cout << "id da ficha: ";
            cin >> id;

            for (auto& f : fichas) {
                if (f.id == id) {
                    ofstream h("historico.txt", ios::app);
                    h << agora() << ";" << f.id << ";" << f.nome << ";"
                      << f.tempoTotal() << ";" << f.caloriasTotal() << "\n";
                    cout << "registrado!\n";
                }
            }
        }

        // menu 4 - exibe histórico de treinos
        else if (op == 4) {
            ifstream h("historico.txt");
            string linha;
            while (getline(h, linha))
                cout << linha << endl;
        }

    } while (op != 0);

    // ao sair, salva exercícios e fichas nos arquivos
    ofstream outE("exercicios.txt");
    for (auto e : exercicios)
        outE << e->serializar() << "\n";

    ofstream outF("fichas.txt");
    for (auto& f : fichas)
        outF << f.serializar() << "\n";

    return 0;
}
