<?php

/**
 * Prace s databazi
 */
abstract class Repository extends Nette\Object
{
    /** @var Nette\Database\Connection */
    protected $connection;

    public function __construct(Nette\Database\Connection $db)
    {
        $this->connection = $db;
    }

    /**
     * Vrati tabulku podle nazvu tridy
     * @return Nette\Database\Table\Selection
     */
    protected function getTable()
    {
        preg_match('#(\w+)Repository$#', get_class($this), $m);
        return $this->connection->table(lcfirst($m[1]));
    }

    /**
     * Cela tabulka
     * @return Nette\Database\Table\Selection
     */
    public function findAll()
    {
        return $this->getTable();
    }

    /**
     * Vraci radky podle filtru
     * @return Nette\Database\Table\Selection
     */
    public function findBy(array $by)
    {
        return $this->getTable()->where($by);
    }
	
     /** @return Nette\Database\Table\ActiveRow */
	 
	public function findById($id)
	{
		return $this->findAll()->get($id);
	}


        /** Vkladani dat */
	public function insert($values)
	{
		return $this->findAll()->insert($values);
	}

}