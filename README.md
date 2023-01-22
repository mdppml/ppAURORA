# ppAURORA

ppAURORA aims to compute the area under curve (AUC). In this study, we use ppAURORA to compute the AUC of the receiver operating characteristic (ROC) curve (AUROC) and precision-recall (PR) curve (AUPR). ppAURORA is based on a 3-party computation framework, called CECILIA, which offers a variety of building blocks to facilitate more complex algorithms in a privacy preserving manner. The project is implemented in C++.

## Installation

No installation is required.

## Compiling

We strongly recommend using cmake to compile the project. To manually compile the project, the followings should help.

### Helper

```bash
c++ -std=gnu++17 -pthread -W -O3 apps/helper.cpp -o helper
```

### AUROC

#### proxy_auroc

```bash
c++ -std=gnu++17 -pthread -W -O3 apps/auroc/proxy.cpp core/Party.h utils/constant.h utils/auc_utils.h utils/parse_options.cpp utils/parse_options.h utils/connection.h utils/flib.h utils/llib.h core/auc.h -o proxy_auroc
```

### AUROC WITH TIE

#### proxy_auroc_tie

```bash
c++ -std=gnu++17 -pthread -W -O3 apps/auroctie/proxy.cpp core/Party.h utils/constant.h utils/auc_utils.h utils/parse_options.cpp utils/parse_options.h utils/connection.h utils/flib.h core/auc.h -o proxy_auroc_tie
```

### AUPR

#### proxy_aupr

```proxy_aupr
c++ -std=gnu++17 -pthread -W -O3 examples/aupr/proxy.cpp core/Party.cpp core/Party.h utils/constant.h utils/auc_utils.h utils/parse_options.cpp utils/parse_options.h utils/connection.h utils/flib.h examples/aupr/llib.h core/auc.h -o proxy_aupr
```

## Usage

```bash
./helper <ip of helper> <port of helper>
./proxy_<auc_type> role <port of proxy 1> <ip of proxy 1> <port of helper> <ip of helper> <delta> <input>
./proxy_<auc_type> role <port of proxy 1> <ip of proxy 1> <port of helper> <ip of helper> <delta> <input>
```

- auc_type = auroc, auroc_tie or aupr
- input = #input parties,#samples of the first input party,#samples of the second input party,...,#samples of the last input party
- delta = delta is a number that specifies how many selections are made after shuffling

Example run for AUROC no-tie is as follows:
```bash
./helper "127.0.0.1" 7777
./proxy_auroc 0 8888 "127.0.0.1" 7777 "127.0.0.1" 10 "8,1000,1000,1000,1000,1000,1000,1000,1000"
./proxy_auroc 1 8888 "127.0.0.1" 7777 "127.0.0.1" 10 "8,1000,1000,1000,1000,1000,1000,1000,1000"
```

# Experiments
In order to run the scalability analysis, one can use the corresponding scripts in _exp_runners.sh_ folder. To run all the experiments at once, _all_exp.sh_ file can be run.

## License
[MIT](https://choosealicense.com/licenses/mit/)
