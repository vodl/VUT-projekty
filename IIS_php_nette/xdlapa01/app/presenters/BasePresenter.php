<?php

use Nette\Application\UI\Presenter;



abstract class BasePresenter extends Presenter
{
    public $sgoutStr = 'Proběhlo odhlášení kvůli nečinosti.';
    /*
     * Obsah menu
     */
    public function beforeRender()
    {
        $this->template->menuItems = array(
            'Domů' => 'Homepage:',
            'Léky' => 'Lek:',
            'Prodej' => 'Prodej:',
            'Import' => 'Import:',
            'Export' => 'Export:',
            'Pojišťovny' => 'Pojistovna:',
            'Uživatelé' => 'Users:'
        );
    }
}
