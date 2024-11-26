# PEMesh

A Graphical Framework to Study the Correlation between Geometric Design and Simulation.

<p align="center"><img src="pemesh_rep_image.png" width="750"></p>

## Dependencies

The framework depends on:
- the cinolib library (commit 00b270d), which is included as a submodule ;
- Triangle library, which is included as a submodule;
- Qt framework, including Qt Charts module, which is assumed to be installed on the local machine (version 5.12.0);
- Boost library, which is assumed to be installed on the local machine.
- Docker App, which is assumed to be installed on the local machine, and open when using PEMesh.

Note that, in the settings of the Docker App, a "Resource Saver" mode is activated by default after some inactivity time. 
This option should be disabled for the correct functioning of PEMesh.

## How to make it work

Please, use --recursive when cloning this repository.

`git clone --recursive https://github.com/DanielaCabiddu/PEMesh`

In the following, please consider `${REPO_ROOT}` variable as the folder where this README.md lies.
PMesh can be built by running either QMake or CMake. 
In both cases, the PMesh executable will be available in the `${REPO_ROOT}/build` folder.

### QMake 

First, build triangle library

`cd ${REPO_ROOT}/external/triangle`\
`mkdir build`\
`cd build`\
`cmake ..`\
`make`

Then, build PEMesh by running the following commands:

`mkdir -p ${REPO_ROOT}/build`\
`cd build`\
`qmake ../src/PMesh.pro`

### CMake

Run the following commands:

`mkdir -p ${REPO_ROOT}/build`\
`cd build`\
`cmake ../src`\
`make`

Triangle library will be automatically built by CMake.

### Docker

In both cases, after the installation open Docker App and run the following commands:

`cd ${REPO_ROOT}/solver`\
`sudo docker build --no-cache -f Dockerfile --target siggraph_2024_solver . -t siggraph_2024_solver:1.0.0`

## Other Authors
* Giuseppe Patanè (CNR IMATI), Michela Spagnuolo (CNR IMATI)

## Citing us
PEMesh has been published as a Software Paper (https://diglib.eg.org/bitstream/handle/10.2312/stag20221251/011-019.pdf?sequence=1&isAllowed=y).

If you use PEMesh in your academic projects, please consider citing it using the following BibTeX entries:

```bibtex
@inproceedings {pemesh-stag2022,
booktitle = {Smart Tools and Applications in Graphics - Eurographics Italian Chapter Conference},
editor = {Cabiddu, Daniela and Schneider, Teseo and Allegra, Dario and Catalano, Chiara Eva and Cherchi, Gianmarco and Scateni, Riccardo},
title = {{A Graphical Framework to Study the Correlation between Geometric Design and Simulation}},
author = {Cabiddu, Daniela and Patané, Giuseppe and Spagnuolo, Michela},
year = {2022},
publisher = {The Eurographics Association},
ISSN = {2617-4855},
ISBN = {978-3-03868-191-5},
DOI = {10.2312/stag.20221251}
}

@article{benchmark-mcs,
title = {Benchmarking the geometrical robustness of a Virtual Element Poisson solver},
journal = {Mathematics and Computers in Simulation},
volume = {190},
pages = {1392-1414},
year = {2021},
issn = {0378-4754},
doi = {https://doi.org/10.1016/j.matcom.2021.07.018},
url = {https://www.sciencedirect.com/science/article/pii/S0378475421002706},
author = {Marco Attene and Silvia Biasotti and Silvia Bertoluzza and Daniela Cabiddu and Marco Livesu and Giuseppe Patanè and Micol Pennacchio and Daniele Prada and Michela Spagnuolo},
keywords = {Polytopal Element Methods, Virtual Element Methods, Polygonal meshes, Geometric metrics, Geometry-PEM correlation}
}

@inbook{vem-and-the-mesh,
author={ Sorgente, Tommaso and Prada, Daniele and  Cabiddu, Daniela and Biasotti, Silvia and  Patanè, Giuseppe and Pennacchio, Micol  and  Bertoluzza, Silvia and  Manzini, Gianmarco and  Spagnuolo, Michela},
title={VEM and the Mesh},
year={2022},
publisher={Springer Nature Switzerland},
address={Basel},
volume={31},
pages={1--57},
booktitle={The Virtual Element Method and its Applications},
url={https://link.springer.com/book/10.1007/978-3-030-95319-5},
series={SEMA SIMAI Springer Series},
isbn={978-3-030-95318-8},
issn={2199-3041},
doi={10.1007/978-3-030-95319-5},
affiliation={IMATI-CNR, Genova, Pavia, Italy},
language={eng}
}

@inproceedings{sorgente2023survey,
  title={A survey of indicators for mesh quality assessment},
  author={Sorgente, Tommaso and Biasotti, Silvia and Manzini, Gianmarco and Spagnuolo, Michela},
  booktitle={Computer graphics forum},
  volume={42},
  number={2},
  pages={461--483},
  year={2023},
  organization={Wiley Online Library}
}
```

If you use the VEM quality indicator, please cite:

```bibtex
@article{sorgente2022role,
  title={The role of mesh quality and mesh quality indicators in the virtual element method},
  author={Sorgente, Tommaso and Biasotti, Silvia and Manzini, Gianmarco and Spagnuolo, Michela},
  journal={Advances in Computational Mathematics},
  volume={48},
  number={1},
  pages={3},
  year={2022},
  publisher={Springer}
}
```

If you use the Mesh Optimization algorithm, please cite:

```bibtex
@article{sorgente2024mesh,
  title={Mesh Optimization for the Virtual Element Method: How Small Can an Agglomerated Mesh Become?},
  author={Sorgente, Tommaso and Vicini, Fabio and Berrone, Stefano and Biasotti, Silvia and Manzini, Gianmarco and Spagnuolo, Michela},
  journal={Journal of Computational Physics},
  pages={113552},
  year={2024},
  publisher={Elsevier}
}
}
```

## Acknowledgements
This paper has been realized in the framework of ERC Project CHANGE (https://cordis.europa.eu/project/id/694515), which has received funding from the European Research Council (ERC) under the European Union’s Horizon 2020 research and innovation programme (grant agreement No 694515).

Special thanks to Marco Livesu for sharing with us the source code to generate parametric polygons and compute geometric metrics.
