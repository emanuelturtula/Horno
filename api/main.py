from fastapi import FastAPI
from fastapi.encoders import jsonable_encoder
from pydantic import BaseModel

app = FastAPI()

class Horno(BaseModel):
    temperature: float = 25
    conversion_factor: float = 5

horno_db = {
    "horno": {"temperature": 25, "conversion_factor": 5}
}

@app.get("/horno/", response_model=Horno)
async def read_temp():
    return horno_db["horno"]

# @app.put("/horno/", response_model=Horno)
# async def update_item(data: Horno):
#     update_horno_encoded = jsonable_encoder(data)
#     horno = update_horno_encoded
#     return horno

@app.patch("/horno/", response_model=Horno)
async def update_item(data: Horno):
    stored_item_data = horno_db["horno"]
    stored_item_model = Horno(**stored_item_data)
    update_data = data.dict(exclude_unset=True)
    updated_item = stored_item_model.copy(update=update_data)
    horno_db["horno"] = jsonable_encoder(updated_item)
    return updated_item
