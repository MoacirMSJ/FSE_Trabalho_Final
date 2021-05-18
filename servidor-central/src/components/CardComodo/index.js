import React from 'react';
import mqtt from 'mqtt';
import './styles.css'

const id  = 'f0:8:d1:c5:c6:e4'

function CardComodo(props) {
  console.log(props)

  const [messages, setMessages] = React.useState({});
  const [temperatura, setTemperatura] =  React.useState(0);
  const [umidade, setUmidade] =  React.useState(0);
  const [estado, setEstado] =  React.useState(0);
  const [lampada, setLampada] =  React.useState(0);
  const [alarme, setAlarme] =  React.useState(0);

  const client = mqtt.connect('wss://test.mosquitto.org:8081');

  React.useEffect(() => {
    
    client.on('connect', function () { 
      client.subscribe(`fse2020/170080366/${props.comodo}/#`, 
          function (err) {
                   if (!err) {
                      console.log(`fse2020/170080366/${props.comodo}/#`)
                      console.log('Conectado')}
          })   
    })
  }, []);

  const gerenciaLampada= ()=>{
    
    setLampada(!Boolean(lampada))
    client.publish(
      `fse2020/170080366/dispositivos/${id}`,
      `{ "lampada": ${lampada}}`
    )
    console.log(`fse2020/170080366/dispositivos/${id}`)
  }

  const gerenciaAlarme= ()=>{
    setAlarme(!Boolean(alarme))
    client.publish(
      `fse2020/170080366/dispositivos/${id}`,
      `{ "alarme": ${alarme}}`
    )
    console.log(`fse2020/170080366/dispositivos/${id}`)
  }

  const gerenciaMensagem =(topic, payload)=>{
    const m = JSON.parse(payload);
    if(topic === `fse2020/170080366/${props.comodo}/temperatura` ){
      if(m.temperatura !== -1){
        setTemperatura(m.temperatura);
      }
      
      // console("Temp: ",temperatura)
    }else if(topic === `fse2020/170080366/${props.comodo}/umidade`){
      if(m.umidade !== -1){
        setUmidade(m.umidade);  
      }
      
      // console("umidade: ",umidade)
    }
    else if(topic === `fse2020/170080366/${props.comodo}/estado`){
      setEstado(m.estado);
    }
  }
  
  client.on('message',gerenciaMensagem);

  return (
    <div className ='container'>
      <h1 className = "title">
          {props.comodo}

       </h1>

      <h2>Temperatura: {temperatura}°C</h2>
      <h2>umidade: {umidade}%</h2>
      
      <div className="sub-container">
        <h2>lampada</h2>
      
        <input
            type="checkbox"
            defaultChecked={lampada}
  
            onChange={gerenciaLampada}
          />
      
      </div>
      <div className="sub-container">
        <h2>alarme</h2>
        <input
            type="checkbox"
            defaultChecked={alarme}

            onChange={gerenciaAlarme}
          />
      </div>
    </div>
    
  );
}

export default CardComodo;
