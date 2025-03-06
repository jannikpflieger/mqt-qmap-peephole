
#include "cliffordsynthesis/PeepholeConfiguration.hpp"
#include "cliffordsynthesis/Results.hpp"
#include "cliffordsynthesis/Tableau.hpp"
#include "ir/QuantumComputation.hpp"

namespace cs {

class PeepholeSynthesizer final{

    public:
        PeepholeSynthesizer() = default;
        PeepholeSynthesizer(Tableau initial, Tableau target)
            : initialTableau(std::move(initial)), targetTableau(std::move(target)) {}
        PeepholeSynthesizer(qc::QuantumComputation& qc)
            : initialTableau(qc.getNqubits(), false),
              targetTableau(qc, 0, std::numeric_limits<std::size_t>::max(), false),
              initialCircuit(std::make_shared<qc::QuantumComputation>(qc)) {}
        ~PeepholeSynthesizer() = default;


        void synthesize(const PeepholeConfiguration& config = {});

        [[nodiscard]] Results& getResults() { return results; };
        [[nodiscard]] qc::QuantumComputation& getResultCircuit();
        [[nodiscard]] Tableau& getResultTableau();


    protected:
        Tableau initialTableau;
        Tableau targetTableau;
        std::shared_ptr<qc::QuantumComputation> initialCircuit;

        PeepholeConfiguration configuration;
        
        Results results;


};

} // namespace cs