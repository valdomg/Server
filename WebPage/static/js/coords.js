
// Últimos valores de coordenada passados pelo Flask
function coords(){      

    console.log(`Aqui`)
    const socket = io();
    const pointer = document.getElementById("pointer");

    socket.on('nova_coordenada', data => {
        const x = data.x;
        const y = data.y;

        const max = 4095;
        const center = 150;
        const offset = 40;

        const normX = center + (x - max/2) * (center - offset) / (max/2);
        const normY = center + (y - max/2) * (center - offset) / (max/2);

        pointer.style.left = `${normX}px`;
        pointer.style.top = `${normY}px`;

        
    });

}