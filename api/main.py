from fastapi import FastAPI
from fastapi.encoders import jsonable_encoder
from pydantic import BaseModel
from fastapi.middleware.cors import CORSMiddleware

app = FastAPI()

origins = [
    "*"
]

app.add_middleware(
    CORSMiddleware,
    allow_origins=origins,
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

class Horno(BaseModel):
    temperature: float = 25
    max_temperature: int = 230
    min_temperature: int = 80
    max_voltage: float = 1.77
    min_voltage: float = 1.16
    samples: int = 100
    analog_reference: float = 3.3

horno_db = {
    "horno": {
        "temperature": 25, 
        "max_temperature": 230,
        "min_temperature": 80,
        "max_voltage": 1.77,
        "min_voltage": 1.16,
        "samples": 100,
        "analog_reference": 3.3
    }
}

@app.get("/horno/", response_model=Horno)
async def read_temp():
    return horno_db["horno"]

@app.patch("/horno/", response_model=Horno)
async def update_item(data: Horno):
    stored_item_data = horno_db["horno"]
    stored_item_model = Horno(**stored_item_data)
    update_data = data.dict(exclude_unset=True)
    updated_item = stored_item_model.copy(update=update_data)
    horno_db["horno"] = jsonable_encoder(updated_item)
    return updated_item
