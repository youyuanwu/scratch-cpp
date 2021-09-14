#include "model.h"

void Item::json_deserialize(std::shared_ptr<Json> j){

    j->GetInt("id",this->id);

    j->GetString("description",this->description);

    // get bool?
    // std::shared_ptr<Json> node = j->GetObj("ok");
}