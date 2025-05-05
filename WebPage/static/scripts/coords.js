
// Últimos valores de coordenada passados pelo Flask
export function coords(x, y){      
    const x = x
    const y = y

    // Normaliza para a área de 300x300 (com centro em 150,150)
    const center = 150;
    const max = 4095; // faixa do ADC, ajuste conforme seu hardware

    const normX = center + (x - max/2) * (center - 40) / (max/2);
    const normY = center + (y - max/2) * (center - 40) / (max/2);

    const pointer = document.getElementById('pointer');
    pointer.style.left = `${normX}px`;
    pointer.style.top = `${normY}px`;

}