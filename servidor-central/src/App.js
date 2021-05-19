import React from 'react';
import CardComodo from './components/CardComodo/index.js';
import mqtt from 'mqtt';

import './styles.css';

function App() {
  const [macComodo, setmacComodo] = React.useState("");
  const [comodo, setComodo] = React.useState([]);
  const [cadatro, setCadastro] = React.useState(0);
  const [formData, setFormData] = React.useState("");
  const client = mqtt.connect('wss://test.mosquitto.org:8081');
 

  const sub = ()=>{
    client.subscribe('fse2020/170080366/dispositivos/#', 
    function (err) {
              if (!err) {
                console.log('Conectado')}
      })   
   }


  React.useEffect(() => {
    sub()
  }, []);

  const trataCadastros =(topic, payload)=>{
    
    const responseEsp = JSON.parse(payload)
    console.log(responseEsp)
    if(responseEsp.cadastro){
      setCadastro(1);
      setmacComodo(responseEsp.id)
      console.log("mac")
      console.log(macComodo)
      //setComodo(prev=>[...prev, ])
      //console.log(m.id)
    }

  }
  
  client.on('message',trataCadastros);

  const handleSubmit = event => {

    
    console.log("submit")
    console.log(macComodo)
    console.log(`fse2020/170080366/dispositivos/${macComodo}`)
    
    setCadastro(0)
    event.preventDefault();
    setComodo([...comodo, {"id": macComodo, "nome":formData}])
    client.publish(
      `fse2020/170080366/dispositivos/${macComodo}`,
      `{ "tipo": 0, "comodo": \"${formData}\"}`
    )
  }


  return (
    <>
    {
      cadatro == 1 ? 
      (
        <div>
          <form onSubmit={handleSubmit}>
              <label>
                <p>digite o nome do novo comodo</p>
                <input name="comodo" onChange={(e) => setFormData(e.target.value)}/>
              </label>
            <button type="submit">enviar</button>
          </form>
        </div>
        
      ) 
      :
      (
        <div className="container-comodo">
          {
            comodo.map((comodo) => (
                <CardComodo comodo={comodo.nome} id={comodo.id} key={comodo.id}/>
          ))
          }
        </div>
      )

    }
        
    </>
  );
}

export default App;
