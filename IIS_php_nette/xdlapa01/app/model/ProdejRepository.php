<?php

class ProdejRepository extends Repository
{


    protected function getTable()
    {
        return $this->connection->table('prodej');
    }
    
    /*
     * Vytvori novy radek databaze s prodejem
     */
    public function createProdej($pojcislo)
{
        return $this->insert(
                array('datum' => new \DateTime(),
                    'pojistovna_cislo' => $pojcislo)              
                );
}


}