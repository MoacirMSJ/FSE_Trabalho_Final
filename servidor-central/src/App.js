import React from 'react';
import CardComodo from './components/CardComodo/index.js';
import mqtt from 'mqtt';

function App() {
  const [messages, setMessages] = React.useState({});
  const [comodo, setComodo] = React.useState([]);
  const [cadatro, setCadastro] = React.useState(0);
  const [formData, setFormData] = React.useState("");
  const client = mqtt.connect('wss://test.mosquitto.org:8081');

  let auxComodo;

  React.useEffect(() => {
    setComodo([{"teste": "teste"}, {"teste": "teste"}])
    setCadastro(1);
    client.on('connect', function () { 
      client.subscribe('fse2020/170080366/dispositivos/#', 
          function (err) {
                   if (!err) {
                      console.log('Conectado')}
          })   
    })
  }, []);

  const trataCadastros =(topic, payload)=>{
    const responseEsp = JSON.parse(payload)
    if(responseEsp.cadastro){
      setCadastro(1);
      auxComodo= {"id": responseEsp.id}
      //setComodo(prev=>[...prev, ])
      //console.log(m.id)
    }

  }
  
  client.on('message',trataCadastros);

  const handleSubmit = event => {
    console.log(event.target.value)
    console.log(formData)
    //setCadastro(1)
  }

  const handleChange = event => {

    // //setFormData(event.target.value);
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
        <div>
          {
            comodo.map((comodo) => (
                <CardComodo comodo="Banheiro" key={comodo.id}/>
          ))
          }
        </div>
      )

    }
        
    </>
  );
}

export default App;
