#include "Variant.h"
#include "split.h"
#include <string>
#include <sstream>
#include <iostream>

using namespace std;
using namespace vcf;

int countAlts(Variant& var) {
    int alts = 0;
    for (map<string, map<string, string> >::iterator s = var.samples.begin(); s != var.samples.end(); ++s) {
        map<string, string>& sample = s->second;
        map<string, string>::iterator gt = sample.find("GT");
        if (gt != sample.end()) {
            map<string, int> genotype = decomposeGenotype(gt->second);
            for (map<string, int>::iterator g = genotype.begin(); g != genotype.end(); ++g) {
                if (g->first != "0" && g->first != ".") {
                    alts += g->second;
                }
            }
        }
    }
    return alts;
}

int main(int argc, char** argv) {

    if (argc != 2) {
        cerr << "usage: " << argv[0] << " <vcf file>" << endl
             << "outputs a VCF stream where AC and NS have been generated for each record using sample genotypes" << endl;
        return 1;
    }

    string filename = argv[1];

    VariantCallFile variantFile;
    if (filename == "-") {
        variantFile.open(std::cin);
    } else {
        variantFile.open(filename);
    }

    if (!variantFile.is_open()) {
        cerr << "could not open " << filename << endl;
        return 1;
    }

    Variant var(variantFile);

    // TODO check if AC is present
    // ensure that AC is listed as an info field
    string line = "##INFO=<ID=AC,Number=1,Type=Integer,Description=\"Total number of alternate alleles in called genotypes\">";
    variantFile.addHeaderLine(line);

    // write the new header
    cout << variantFile.header << endl;
 
    // print the records, filtering is done via the setting of varA's output sample names
    while (variantFile.getNextVariant(var)) {
        stringstream ac;
        ac << countAlts(var);
        var.info["AC"] = ac.str();
        stringstream ns;
        ns << var.samples.size();
        var.info["NS"] = ns.str();
        cout << var << endl;
    }

    return 0;

}

