#include <iostream>
#include <vector>
#include <string>
#include <fstream>
using namespace std;

// =================== CLASSE BASE ===================
class Exercicio {
protected:
    int id;
    string nome;

public:
    Exercicio() {}
    Exercicio(int id, const string& nome) : id(id), nome(nome) {}
    virtual ~Exercicio() {}

    int getId() const { return id; }
    string getNome() const { return nome; }

    virtual double calcularCalorias() const = 0;
    virtual double calcularTempo() const = 0;

    virtual void salvar(ofstream& arq) const = 0;
};

// =================== CARDIO ===================
class Cardio : public Exercicio {
    double distancia;
    double velocidade;

public:
    Cardio(int id, const string& nome, double distancia, double velocidade)
        : Exercicio(id, nome), distancia(distancia), velocidade(velocidade) {}

    double calcularCalorias() const override {
        return distancia * 5.0;  // simplificado
    }

    double calcularTempo() const override {
        return distancia / velocidade;
    }

    void salvar(ofstream& arq) const override {
        arq << "CARDIO;" << id << ";" << nome << ";" 
            << distancia << ";" << velocidade << "\n";
    }
};

// =================== FORÇA ===================
class Forca : public Exercicio {
    int repeticoes;
    double carga;

public:
    Forca(int id, const string& nome, int repeticoes, double carga)
        : Exercicio(id, nome), repeticoes(repeticoes), carga(carga) {}

    double calcularCalorias() const override {
        return repeticoes * carga * 0.1;
    }

    double calcularTempo() const override {
        return repeticoes * 2; // simplificado
    }

    void salvar(ofstream& arq) const override {
        arq << "FORCA;" << id << ";" << nome << ";" 
            << repeticoes << ";" << carga << "\n";
    }
};

// =================== FICHA ===================
struct Ficha {
    int idFicha;
    string nome;
    vector<int> idsExercicios;

    void salvar(ofstream& arq) const {
        arq << idFicha << ";" << nome << ";";
        for (int i = 0; i < (int)idsExercicios.size(); i++) {
            arq << idsExercicios[i];
            if (i < (int)idsExercicios.size() - 1) arq << ",";
        }
        arq << "\n";
    }
};

// ================= HISTÓRICO ==================
struct HistoricoItem {
    string nomeAluno;
    int idFicha;
    string data;

    void salvar(ofstream& arq) const {
        arq << nomeAluno << ";" << idFicha << ";" << data << "\n";
    }
};

// =============================================================
//                       ARMAZENAMENTO
// =============================================================
vector<Cardio> listaCardio;
vector<Forca> listaForca;
vector<Ficha> listaFichas;
vector<HistoricoItem> historico;

int proximoId = 1;
int proximoIdFicha = 1;

// procura exercício por id
Exercicio* buscarExercicio(int id) {
    for (auto& c : listaCardio) if (c.getId() == id) return &c;
    for (auto& f : listaForca) if (f.getId() == id) return &f;
    return nullptr;
}

// =============================================================
//                         FUNÇÕES
// =============================================================
void cadastrarExercicio() {
    int tipo;
    cout << "1 - Cardio\n2 - Forca\n> ";
    cin >> tipo;

    string nome;
    cout << "Nome: ";
    cin.ignore();
    getline(cin, nome);

    if (tipo == 1) {
        double dist, vel;
        cout << "Distancia (km): ";
        cin >> dist;
        cout << "Velocidade (km/h): ";
        cin >> vel;

        listaCardio.emplace_back(proximoId++, nome, dist, vel);
    } else {
        int rep;
        double carga;
        cout << "Repeticoes: ";
        cin >> rep;
        cout << "Carga (kg): ";
        cin >> carga;

        listaForca.emplace_back(proximoId++, nome, rep, carga);
    }

    cout << "Exercicio cadastrado!\n";
}

void criarFicha() {
    Ficha f;
    f.idFicha = proximoIdFicha++;

    cout << "Nome da ficha: ";
    cin.ignore();
    getline(cin, f.nome);

    cout << "Insira IDs dos exercícios (0 para encerrar):\n";

    while (true) {
        cout << "> ";
        int id;
        cin >> id;
        if (id == 0) break;
        if (buscarExercicio(id)) {
            f.idsExercicios.push_back(id);
        } else {
            cout << "ID inválido!\n";
        }
    }

    listaFichas.push_back(f);
    cout << "Ficha criada com sucesso!\n";
}

void registrarTreino() {
    HistoricoItem h;

    cout << "Nome do aluno: ";
    cin.ignore();
    getline(cin, h.nomeAluno);

    cout << "ID da ficha: ";
    cin >> h.idFicha;

    cout << "Data: ";
    cin.ignore();
    getline(cin, h.data);

    historico.push_back(h);
    cout << "Treino registrado!\n";
}

void salvarArquivos() {
    ofstream arqEx("exercicios.txt");
    for (auto& c : listaCardio) c.salvar(arqEx);
    for (auto& f : listaForca) f.salvar(arqEx);
    arqEx.close();

    ofstream arqFi("fichas.txt");
    for (auto& f : listaFichas) f.salvar(arqFi);
    arqFi.close();

    ofstream arqHis("historico.txt");
    for (auto& h : historico) h.salvar(arqHis);
    arqHis.close();

    cout << "Arquivos salvos!\n";
}

void listarExercicios() {
    cout << "\n== CARDIO ==\n";
    for (auto& c : listaCardio)
        cout << c.getId() << " - " << c.getNome() << "\n";

    cout << "\n== FORCA ==\n";
    for (auto& f : listaForca)
        cout << f.getId() << " - " << f.getNome() << "\n";
}

void listarFichas() {
    for (auto& f : listaFichas) {
        cout << f.idFicha << " - " << f.nome << " : ";
        for (int id : f.idsExercicios) cout << id << " ";
        cout << "\n";
    }
}

void listarHistorico() {
    for (auto& h : historico) {
        cout << h.nomeAluno << " - Ficha " << h.idFicha
             << " - " << h.data << "\n";
    }
}

// =============================================================
//                              MAIN
// =============================================================
int main() {
    int op;

    do {
        cout << "\n===== MENU =====\n"
             << "1 - Cadastrar exercicio\n"
             << "2 - Criar ficha\n"
             << "3 - Registrar treino\n"
             << "4 - Listar exercicios\n"
             << "5 - Listar fichas\n"
             << "6 - Listar historico\n"
             << "7 - Salvar tudo\n"
             << "0 - Sair\n> ";

        cin >> op;

        switch (op) {
            case 1: cadastrarExercicio(); break;
            case 2: criarFicha(); break;
            case 3: registrarTreino(); break;
            case 4: listarExercicios(); break;
            case 5: listarFichas(); break;
            case 6: listarHistorico(); break;
            case 7: salvarArquivos(); break;
        }

    } while (op != 0);

    return 0;
}
