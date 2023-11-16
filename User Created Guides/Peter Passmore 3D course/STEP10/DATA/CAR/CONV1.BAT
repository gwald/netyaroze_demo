dxf2rsd -o specky0.rsd -cf specky0.cl -g -quad1 -quad2 20 -back specky0.dxf
dxf2rsd -o specky1.rsd -cf specky0.cl -g -quad1 -quad2 20 -back specky1.dxf
dxf2rsd -o specky2.rsd -cf specky0.cl -g -quad1 -quad2 20 -back specky2.dxf
dxf2rsd -o specky3.rsd -cf specky0.cl -g -quad1 -quad2 20 -back specky3.dxf
dxf2rsd -o specky4.rsd -cf specky0.cl -g -quad1 -quad2 20 -back specky4.dxf
rsdlink -o specky0.tmd specky0.rsd
rsdlink -o specky1.tmd specky1.rsd
rsdlink -o specky2.tmd specky2.rsd
rsdlink -o specky3.tmd specky3.rsd
rsdlink -o specky4.tmd specky4.rsd
