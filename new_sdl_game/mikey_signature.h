#ifndef MIKEY_SIGNATURE
#define MIKEY_SIGNATURE

#include <iostream>
#include <string>
#include <sstream>

namespace mikey {
    std::string ascii_signature() {
        std::stringstream signature;
        signature << "oooooooooooooooooodddddddddddddddddddddooooooooo\n";
        signature << "oooooooooooooooooodddddddddddooooooooooooooooooo\n";
        signature << "oooooooooooooooooooooooolooooloOKOxooooooooooooo\n";
        signature << "oooooooooooolclooooooooooll:;,;xNXoloooooooooooo\n";
        signature << "ooooooooooool,',;;::;;,,''.....;ko'':loooooooooo\n";
        signature << "ooooooooooodl,.................;kd'.';oooooooooo\n";
        signature << "ooooooooooodl,',,'.............oNNd'':oooooooooo\n";
        signature << "ooooooooooodl;,;;::;,.........'o00o:cooooooooooo\n";
        signature << "ooooooooooooo:,::::::;;;;::::::::clooooooooooooo\n";
        signature << "oooooooooooooc,;ccc:;;;;;;;;;;;;:loooooooooooooo\n";
        signature << "ooooooooooool;,;:ccccc:::::::::::::coooooooooooo\n";
        signature << "oooooooooo:;;'';:ccccccccccc:::::;'.,coooooooooo\n";
        signature << "oooooooool;'..';:cccllccccccc::;,'..,:oooooooooo\n";
        signature << "ooooooooooc,...',;;:::cccccc:,'...':oxxooooooooo\n";
        signature << "oooooooooool:'....'''',,,,,'..'''';dOkdooooooooo\n";
        signature << "oooooooool:;::;,,,'',,,,,,,'''..   'cllooooooooo\n";
        signature << "ooooooooo;.    'c;:dl;lxxc;lc..;'.'.  'ldooooooo\n";
        signature << "oooooooool'    :Okk0OxO00OO00OO0OOO; .:doooooooo\n";
        signature << "ooooooooool'  .o0OOkkkO0000000Okkkk; ;oooooooooo\n";
        signature << "oooooooooodl.  ;dc;;,,;codkkxl;,,...'ldooooooooo\n";
        signature << "ooooooooooool' .;llc::lodxxkxdlc:'.,cooooooooooo\n";
        signature << "oooooooooooooc. 'd00000xoodddkOdc:cooooooooooooo\n";
        signature << "ooooooooooooo,   .,:clolllllccc:cooooooooooooooo\n";
        signature << "ooooooooooool.              .:odoooooooooooooooo\n";
        signature << "https://www.ascii-art-generator.org\n";
        return signature.str();
    }
}

#endif