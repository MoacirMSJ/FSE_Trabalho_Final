import React from 'react';

import './styles.css'
function CardComodo() {
  return (
    <div className ='container'>
      <h1 className = "title">
          comodo: Sala 
       </h1>

      <h2>Temperatura: 30</h2>
      <h2>umidade: 30%</h2>
      
      <div className="sub-container">
        <h2>lampada</h2>
      </div>
      <div className="sub-container">
        <h2>alarme</h2>
      </div>
    </div>
    
  );
}

export default CardComodo;
