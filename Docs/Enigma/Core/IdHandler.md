# ID Handler
Many systems in Enigma require a list of objects. The only problem with this is that you can't just store a index into a list, because if anything gets added to that list then that index could point to a differant object than intended. To fix this problem I created the [ID Handler class](#id-handler-class). How it works will be explained later.

### ID struct
The core of the ID system is the ID struct
| Name | Type | Description |
| ---- | ---- | ----------- |
| index | `uint32_t` | A index into a list |
| generation | `uint32_t` | The number of times the index had been used |

The ID struct also has the `InvalidID` function. this is a static function that returns a invalid ID, where the index and generation are -1.

### ID Handler Class
#### Note: The current implementation of IDHandler is not up to date with the docs
The [ID struct](#id-struct) by itself isn't very useful. To use the [ID struct](#id-struct) you need to use the `IDHandler` class. The `IDHandler` class makes sure every object in its list stays in the same position so every [ID](#id-struct) stays valid. If a object gets removed from the list then any [ID](#id-struct) pointing to that object is now invalid. This is what the `generation` variable in the [ID struct](#id-struct) is for. When a object gets removed the generation at that objects slot gets increased. When you try to index into the list the `IDHandler` checks the `generation` of the [ID](#id-struct) you pass in, against the `generation` of the slot. If they aren't the same then we know the [ID](#id-struct) is invalid.

The actual `IDHandler` class is a template class. It represents all data as pointers.
#### Functions
| Name | Description |
| ---- | ----------- |
| `std::vector<T*>& GetData()` | Gets the list of data |
| `bool IsValid(ID id)` | Returns true if [ID](#id-struct) is valid |
| `ID Create(T* value)` | Adds value to the list and returns a [ID](#id-struct) to it |
| `void Delete(ID id)` | Deletes the object at id in the list |
| `T* Get(ID id)` | Returns the value at id |
| `ID Get(int index)` | Returns the [ID](#id-struct) that correlates to index into the list |
| `ID Get(T* value)` | Returns the [ID](#id-struct) of value if it is in the list, if its not in the list then returns [InvalidID](#id-struct) |
| `void Clear()` | Deletes all data from the list and resets all slot generations |
#### Member Variables
| Name | Type | Description |
| ---- | ---- | ----------- |
| m_Data | `std::vector<T*>` | All of the data |
| m_IDs | `std::map<ID, std::pair<int, uint32_t>>` | Maps all IDs to a index into m_Data and stores the generation at that index|
| m_Slots | `std::vector<ID>` | All open slots |

### Example
The following is a example of how you could use the ID System.
```C++
struct Thing {
    std::string name;

    Thing(const std::string& name)
        : name(name) { }
};

void Enigma::Core::ApplicationMain(Enigma::Core::Application* app) {
    IDHandler<Thing> list;
    ID thing1 = list.Create(new Thing("Thing 1"));
    ID thing2 = list.Create(new Thing("Thing 2"));

    std::cout << list.Get(thing1)->name << std::endl;
    std::cout << list.Get(thing2)->name << std::endl;
}
```